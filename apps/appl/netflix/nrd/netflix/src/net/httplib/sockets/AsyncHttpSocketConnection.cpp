/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#ifdef HAVE_FIONREAD
#include <sys/ioctl.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <nrdbase/AseErrorCodes.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdnet/IAsyncHttpListeners.h>
#include "AsyncHttpSocketClient.h"
#include "AsyncHttpSocketConnection.h"
#include <nrdbase/Log.h>

using namespace std;

namespace netflix {

#define ASYNC_IP_V4_INDEX 0
#define ASYNC_IP_V6_INDEX 1
#define ASYNC_IP_INVALID_INDEX 0xFFFFFFFF

const char *stateNames[] = {
    "LOOKING_UP_STATE",   /**< The DNS lookup is in progress. */
    "OPENING_STATE",          /**< The connection is being opened. */
    "IDLE_STATE",             /**< The connection has been opened and there is no request in progress. */
    "ACTIVE_STATE",           /**< The connection has been opened and there is at least one request in
                                 progress. */
    "CLOSED_STATE",           /**< The connection has been closed. */
    "FAILED_STATE"            /**< The connection has failed. */
};

int32_t mapErrno( int error, bool connecting = false )
{
    if (error == EINPROGRESS)
    {
        return AS_IN_PROGRESS;
    }
    else if (error == ECONNREFUSED)
    {
        return AS_CONNECTION_REFUSED;
    }
    else if (error == ENETUNREACH)
    {
        return AS_CONNECTION_NET_UNREACHABLE;
    }
    else if (error == ETIMEDOUT)
    {
        return AS_CONNECTION_TIMEOUT;
    }
    else if (error == EHOSTUNREACH)
    {
        return AS_CONNECTION_NO_ROUTE_TO_HOST;
    }
    else if (error == ECONNRESET)
    {
        return connecting ? AS_CONNECTION_RESET_ON_CONNECT : AS_CONNECTION_RESET;
    }
    else if (error == ENETDOWN)
    {
        return AS_CONNECTION_NETWORK_DOWN;
    }

    return AS_CONNECTION_ERROR;
}

AsyncHttpSocketConnection::SocketAttempt::SocketAttempt( AsyncHttpSocketConnection* pAsyncHttpSocketConnection )
    : mAsyncHttpSocketConnectionPtr( pAsyncHttpSocketConnection )
    , mState( SocketAttempt::INIT )
    , mOsSocketHandle( -1 )
{
}

int32_t AsyncHttpSocketConnection::SocketAttempt::open(
                uint32_t            id,
                uint32_t            httpId,
                NrdIpAddr const&    ipAddress,
                uint16_t            port,
                uint32_t            sequence,
                AseTimeVal const&   fastTimeoutDelay,
                uint32_t            socketReceiveBufferSize,
                AseTimeVal const&   dnsLookupTime )
{
    assert( mState == SocketAttempt::INIT );

    NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p opening %s (fast timeout in %llums)",
        mAsyncHttpSocketConnectionPtr,
        ipAddress.getStrAddr().c_str(),
        fastTimeoutDelay.ms() );

    mId                         = id;
    mHttpId                     = httpId;
    mDestinationIpAddress       = ipAddress;
    mDestinationPort            = port;
    mSequence                   = sequence;
    mTcpConnectingTime          = AseTimeVal::now();
    mFastTimeoutDelay           = fastTimeoutDelay;
    mOsSocketReceiveBufferSize  = socketReceiveBufferSize;
    mDnsLookupTime              = dnsLookupTime;

    if ( mAsyncHttpSocketConnectionPtr->mTraceListenerPtr )
        mAsyncHttpSocketConnectionPtr->mTraceListenerPtr->reportTcpStart(
                                            mId,
                                            mHttpId,
                                            mAsyncHttpSocketConnectionPtr->mDestinationHostName,
                                            mDestinationIpAddress.getStrAddr(),
                                            ntohs( mDestinationPort ),
                                            true );



    int32_t retVal = openSocket();
    if ( retVal == AS_NO_ERROR )
    {
        do
        {
            //Set various socket options.

            //Set the socket to non-blocking mode.
            int flags;

            flags = fcntl(
                        mOsSocketHandle,
                        F_GETFL,
                        0);

            if (flags == -1)
            {
                retVal = AS_SOCKET_ERROR;

                mLowLevelFailureCode = errno;

                break;
            }

            retVal = fcntl(
                        mOsSocketHandle,
                        F_SETFL,
                        flags | O_NONBLOCK );

            if (retVal == -1)
            {
                NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %d set non-blocking failed (%d) %s",
                        mAsyncHttpSocketConnectionPtr,
                        mOsSocketHandle,
                        (int) errno,
                        mDestinationIpAddress.getStrAddr().c_str() );

                retVal = AS_SOCKET_ERROR;

                mLowLevelFailureCode = errno;

                break;
            }

            if (mOsSocketReceiveBufferSize > 0)
            {
                //Set the socket receive buffer size.
                retVal = setsockopt(
                            mOsSocketHandle,
                            SOL_SOCKET,
                            SO_RCVBUF,
                            &mOsSocketReceiveBufferSize,
                            sizeof( mOsSocketReceiveBufferSize ) );

                if (retVal == -1)
                {
                    NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %d set socket buffer failed (%d) %s",
                        mAsyncHttpSocketConnectionPtr,
                        mOsSocketHandle,
                        (int) errno,
                        mDestinationIpAddress.getStrAddr().c_str() );

                    retVal = AS_SOCKET_ERROR;

                    mLowLevelFailureCode = errno;

                    break;
                }
                else
                {
                    NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection set socket receive buffer size: %u",
                            mOsSocketReceiveBufferSize);
                }
            }

            socklen_t optLen;

            optLen = sizeof( mOsSocketReceiveBufferSize );

            retVal = getsockopt(
                        mOsSocketHandle,
                        SOL_SOCKET,
                        SO_RCVBUF,
                        &mOsSocketReceiveBufferSize,
                        &optLen );

            if ( retVal != 0 )
            {
                NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %d get socket buffer failed (%d) %s",
                            mAsyncHttpSocketConnectionPtr,
                            mOsSocketHandle,
                            (int) errno,
                            mDestinationIpAddress.getStrAddr().c_str() );

                mLowLevelFailureCode = errno;

                retVal = AS_SOCKET_ERROR;
            }
            else
            {
                NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection socket receive buffer size: %u",
                                mOsSocketReceiveBufferSize);
            }
        }
        while(false);

        if ( retVal == AS_NO_ERROR )
        {
            retVal = connectSocket();
            if ( retVal != AS_NO_ERROR && retVal != AS_IN_PROGRESS )
            {
                    NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %d connect socket failed (%d %d) %s",
                            mAsyncHttpSocketConnectionPtr,
                            mOsSocketHandle,
                            retVal,
                            mLowLevelFailureCode,
                            mDestinationIpAddress.getStrAddr().c_str() );
            }
        }
    }
    else
    {
        NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %d open socket failed (%d %d) %s",
                            mAsyncHttpSocketConnectionPtr,
                            mOsSocketHandle,
                            retVal,
                            mLowLevelFailureCode,
                            mDestinationIpAddress.getStrAddr().c_str() );
    }

    if ( retVal == AS_NO_ERROR )
    {
        mTcpConnectCompleteTime = AseTimeVal::now();

        obtainSourceAddress();

        mState = SocketAttempt::CONNECTED;

        if ( mAsyncHttpSocketConnectionPtr->mTraceListenerPtr )
            mAsyncHttpSocketConnectionPtr->mTraceListenerPtr->reportTcpConnect(
                                            mId,
                                            mHttpId,
                                            mSourceIpAddress.getStrAddr(),
                                            ntohs( mSourcePortNumber ),
                                            mAsyncHttpSocketConnectionPtr->mDestinationHostName,
                                            mDestinationIpAddress.getStrAddr(),
                                            ntohs( mDestinationPort ),
                                            mDnsLookupTime,
                                            mTcpConnectCompleteTime - mTcpConnectingTime );

    }
    else if ( retVal == AS_IN_PROGRESS )
    {
        obtainSourceAddress();

        mState = SocketAttempt::CONNECTING;
    }
    else
    {
        mConnectionFailureCode = retVal;

        mTcpConnectCompleteTime = AseTimeVal::now();

        if ( mOsSocketHandle != -1 )
        {
            ::close( mOsSocketHandle );

            mOsSocketHandle = -1;
        }

        if ( mAsyncHttpSocketConnectionPtr->mTraceListenerPtr )
            mAsyncHttpSocketConnectionPtr->mTraceListenerPtr->reportTcpFailure(
                                            mId,
                                            mHttpId,
                                            mConnectionFailureCode,
                                            mLowLevelFailureCode,
                                            mSourceIpAddress.getStrAddr(),
                                            ntohs( mSourcePortNumber ),
                                            mAsyncHttpSocketConnectionPtr->mDestinationHostName,
                                            mDestinationIpAddress.getStrAddr(),
                                            ntohs( mDestinationPort ),
                                            mDnsLookupTime,
                                            mTcpConnectCompleteTime - mTcpConnectingTime );

        mState = SocketAttempt::FAILED;
    }

    return retVal;
}

int32_t AsyncHttpSocketConnection::SocketAttempt::close( int32_t reason )
{
    if ( mState == SocketAttempt::CONNECTING || mState == SocketAttempt::CONNECTED )
    {
        assert( mOsSocketHandle != -1 );

        ::close( mOsSocketHandle );

        mOsSocketHandle = -1;

        if ( mState == SocketAttempt::CONNECTING )
        {
            if ( mAsyncHttpSocketConnectionPtr->mTraceListenerPtr )
                mAsyncHttpSocketConnectionPtr->mTraceListenerPtr->reportTcpFailure(
                                            mId,
                                            mHttpId,
                                            reason,
                                            0,
                                            mSourceIpAddress.getStrAddr(),
                                            ntohs( mSourcePortNumber ),
                                            mAsyncHttpSocketConnectionPtr->mDestinationHostName,
                                            mDestinationIpAddress.getStrAddr(),
                                            ntohs( mDestinationPort ),
                                            mDnsLookupTime,
                                            AseTimeVal::now() - mTcpConnectingTime );
        }
        else
        {
            if ( mAsyncHttpSocketConnectionPtr->mTraceListenerPtr )
                mAsyncHttpSocketConnectionPtr->mTraceListenerPtr->reportTcpDisconnect(
                                            mId,
                                            mHttpId,
                                            reason,
                                            0,
                                            TCP_DISCONNECT_BY_LOCAL,
                                            false );
        }

        mState = SocketAttempt::CLOSED;
    }
    else if ( mState == SocketAttempt::INIT )
    {
        mState = SocketAttempt::CLOSED;
    }

    return AS_NO_ERROR;
}

int32_t AsyncHttpSocketConnection::SocketAttempt::processSelect( AseTimeVal const& timeNow )
{
    assert( mState == SocketAttempt::CONNECTING );

    int retVal = AS_UNKNOWN_ERROR;

    mTcpConnectCompleteTime = timeNow;

    int optVal;
    int optValLen;

    optValLen = sizeof( optVal );

    // Return any pending errors in the socket. The value returned
    // corresponds to the standard error codes defined in <errno.h>
    if (getsockopt(
                mOsSocketHandle,
                SOL_SOCKET,
                SO_ERROR,
                (void*) &optVal,
                (socklen_t*) &optValLen ) == 0)
    {
        if (optVal == 0)
        {
            obtainSourceAddress();

            mState = SocketAttempt::CONNECTED;

            if ( mAsyncHttpSocketConnectionPtr->mTraceListenerPtr )
                mAsyncHttpSocketConnectionPtr->mTraceListenerPtr->reportTcpConnect(
                                                mId,
                                                mHttpId,
                                                mSourceIpAddress.getStrAddr(),
                                                ntohs( mSourcePortNumber ),
                                                mAsyncHttpSocketConnectionPtr->mDestinationHostName,
                                                mDestinationIpAddress.getStrAddr(),
                                                ntohs( mDestinationPort ),
                                                mDnsLookupTime,
                                                mTcpConnectCompleteTime - mTcpConnectingTime );

            NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %d connected %s (%llums)",
                        mAsyncHttpSocketConnectionPtr,
                        mOsSocketHandle,
                        mDestinationIpAddress.getStrAddr().c_str(),
                        ( mTcpConnectCompleteTime - mTcpConnectingTime ).ms() );

            retVal = AS_NO_ERROR;
        }
        else
        {
            mConnectionFailureCode = mapErrno( optVal, true /*connecting*/ );

            mLowLevelFailureCode = optVal;

            NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %d failed to connect(1) %d %d %s",
                        mAsyncHttpSocketConnectionPtr,
                        mOsSocketHandle,
                        mConnectionFailureCode,
                        mLowLevelFailureCode,
                        mDestinationIpAddress.getStrAddr().c_str() );

            ::close( mOsSocketHandle );

            mOsSocketHandle = -1;

            mState = SocketAttempt::FAILED;

            retVal = mConnectionFailureCode;
        }
    }
    else
    {
        mConnectionFailureCode = AS_SOCKET_ERROR;

        mLowLevelFailureCode = errno;

        NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %d failed to connect(2) %d %d %s",
                        mAsyncHttpSocketConnectionPtr,
                        mOsSocketHandle,
                        mConnectionFailureCode,
                        mLowLevelFailureCode,
                        mDestinationIpAddress.getStrAddr().c_str() );

        ::close( mOsSocketHandle );

        mOsSocketHandle = -1;

        mState = SocketAttempt::FAILED;

        retVal = mConnectionFailureCode;
    }

    if ( retVal != AS_NO_ERROR )
    {
        if ( mAsyncHttpSocketConnectionPtr->mTraceListenerPtr )
            mAsyncHttpSocketConnectionPtr->mTraceListenerPtr->reportTcpFailure(
                                        mId,
                                        mHttpId,
                                        mConnectionFailureCode,
                                        mLowLevelFailureCode,
                                        mSourceIpAddress.getStrAddr(),
                                        ntohs( mSourcePortNumber ),
                                        mAsyncHttpSocketConnectionPtr->mDestinationHostName,
                                        mDestinationIpAddress.getStrAddr(),
                                        ntohs( mDestinationPort ),
                                        mDnsLookupTime,
                                        mTcpConnectCompleteTime - mTcpConnectingTime );
    }

    return retVal;
}

int AsyncHttpSocketConnection::SocketAttempt::transferSocketHandle()
{
    mState = SocketAttempt::CLOSED;

    int result = mOsSocketHandle;

    mOsSocketHandle = -1;

    return result;
}

AsyncHttpSocketConnection::SocketAttempt::~SocketAttempt()
{
    close();
}

AsyncHttpSocketConnection::SocketAttemptIpv4::SocketAttemptIpv4( AsyncHttpSocketConnection* pAsyncHttpSocketConnection )
    : SocketAttempt( pAsyncHttpSocketConnection )
{
}

int32_t AsyncHttpSocketConnection::SocketAttemptIpv4::openSocket()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    //Open a TCP socket.
    mOsSocketHandle = socket( AF_INET, SOCK_STREAM, 0 );

    if (mOsSocketHandle != -1)
    {
        retVal = AS_NO_ERROR;
    }
    else
    {
        mLowLevelFailureCode = errno;

        retVal = AS_SOCKET_ERROR;
    }

    return retVal;
}

int32_t AsyncHttpSocketConnection::SocketAttemptIpv4::connectSocket()
{
    //Connect to the server.
    struct sockaddr_in sockAddrIn;

    sockAddrIn.sin_family = AF_INET;
    sockAddrIn.sin_addr.s_addr = SocketAttempt::mDestinationIpAddress.getInAddr().s_addr;
    sockAddrIn.sin_port = SocketAttempt::mDestinationPort;

    int32_t retVal = connect(
                mOsSocketHandle,
                (const struct sockaddr*) &sockAddrIn,
                sizeof( sockAddrIn ) );

    if (retVal == 0)
    {
        //The connection is successfully established.
        retVal = AS_NO_ERROR;
    }
    else
    {
        retVal = mapErrno( errno, true /*connecting*/ );
    }

    if ((retVal != AS_NO_ERROR) && (retVal != AS_IN_PROGRESS))
    {
        //The connection failed.
        mLowLevelFailureCode = errno;

        NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %d failed to connect(3) %d %d %s",
                        mAsyncHttpSocketConnectionPtr,
                        mOsSocketHandle,
                        retVal,
                        mLowLevelFailureCode,
                        mDestinationIpAddress.getStrAddr().c_str() );

        ::close( mOsSocketHandle );

        mOsSocketHandle = -1;
    }

    return retVal;
}

void AsyncHttpSocketConnection::SocketAttemptIpv4::obtainSourceAddress()
{
    struct sockaddr_in srcAddrIn;

    socklen_t srcAddrInLen = sizeof( srcAddrIn );

    if (getsockname(
                mOsSocketHandle,
                (struct sockaddr*) &srcAddrIn,
                &srcAddrInLen ) == 0)
    {
        mSourceIpAddress.setInAddr( srcAddrIn.sin_addr );
        mSourcePortNumber = srcAddrIn.sin_port;
    }
    else
    {
        NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %d getsockname failed (%d) %s",
                        mAsyncHttpSocketConnectionPtr,
                        mOsSocketHandle,
                        (int) errno,
                        mDestinationIpAddress.getStrAddr().c_str() );

    }
}

#ifdef NRDP_HAS_IPV6
AsyncHttpSocketConnection::SocketAttemptIpv6::SocketAttemptIpv6( AsyncHttpSocketConnection* pAsyncHttpSocketConnection )
    : SocketAttempt( pAsyncHttpSocketConnection )
{
}

int32_t AsyncHttpSocketConnection::SocketAttemptIpv6::openSocket()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    //Open a TCP socket.
    mOsSocketHandle = socket( AF_INET6, SOCK_STREAM, 0 );

    if (mOsSocketHandle != -1)
    {
        //Set to IPv6 only.
        int ipV6OnlyFlag = 1;

        retVal = setsockopt(
                    mOsSocketHandle,
                    IPPROTO_IPV6,
                    IPV6_V6ONLY,
                    &ipV6OnlyFlag,
                    sizeof( int ) );

        if (retVal == -1)
        {
            mLowLevelFailureCode = errno;

            ::close( mOsSocketHandle );

            mOsSocketHandle = -1;

            retVal = AS_SOCKET_ERROR;
        }
        else
        {

        }   retVal = AS_NO_ERROR;
    }
    else
    {
        mLowLevelFailureCode = errno;

        retVal = AS_SOCKET_ERROR;
    }

    return retVal;
}

int32_t AsyncHttpSocketConnection::SocketAttemptIpv6::connectSocket()
{
    //Connect to the server.
    struct sockaddr_in6 sockAddrIn;

    sockAddrIn.sin6_family = AF_INET6;
    memcpy(
        sockAddrIn.sin6_addr.s6_addr,
        mDestinationIpAddress.getIn6Addr().s6_addr,
        16 );
    sockAddrIn.sin6_port = mDestinationPort;
    sockAddrIn.sin6_flowinfo = 0;

    if (!mDestinationIpAddress.getIn6ScopeId().empty())
    {
        sockAddrIn.sin6_scope_id = strtoul(
                        mDestinationIpAddress.getIn6ScopeId().c_str(),
                        NULL,
                        10 );
    }
    else
    {
        sockAddrIn.sin6_scope_id = 0;
    }

    int32_t retVal = connect(
                mOsSocketHandle,
                (const struct sockaddr*) &sockAddrIn,
                sizeof( sockAddrIn ) );

    if (retVal == 0)
    {
        //The connection is successfully established.
        retVal = AS_NO_ERROR;
    }
    else
    {
        retVal = mapErrno( errno, true /*connecting*/ );
    }

    if ((retVal != AS_NO_ERROR) && (retVal != AS_IN_PROGRESS))
    {
        //The connection failed.
        mLowLevelFailureCode = errno;

        NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %d failed to connect(4) %d %d %s",
                        mAsyncHttpSocketConnectionPtr,
                        mOsSocketHandle,
                        retVal,
                        mLowLevelFailureCode,
                        mDestinationIpAddress.getStrAddr().c_str() );

        ::close( mOsSocketHandle );

        mOsSocketHandle = -1;
    }

    return retVal;
}

void AsyncHttpSocketConnection::SocketAttemptIpv6::obtainSourceAddress()
{
    struct sockaddr_in6 srcAddrIn;

    socklen_t srcAddrInLen = sizeof( srcAddrIn );

    if (getsockname(
                mOsSocketHandle,
                (struct sockaddr*) &srcAddrIn,
                &srcAddrInLen ) == 0)
    {
        mSourceIpAddress.setIn6Addr( srcAddrIn.sin6_addr );

        if (srcAddrIn.sin6_scope_id != 0)
        {
            char tempStr[32];

            snprintf(
                tempStr,
                32,
                "%u",
                srcAddrIn.sin6_scope_id );

            mSourceIpAddress.setIn6ScopeId( tempStr );
        }

        mSourcePortNumber = srcAddrIn.sin6_port;
    }
}

#endif

/** Constructor. */
AsyncHttpSocketConnection::AsyncHttpSocketConnection(
                                AsyncHttpSocketClient* pAsyncHttpSocketClient,
                                uint32_t id,
                                const std::string& destinationHostName,
                                IHttpConnectionTraceListener* pTraceListener ) :
                                            mAsyncHttpSocketClientPtr( pAsyncHttpSocketClient ),
                                            mId( id ),
                                            mHttpId( id ),
                                            mDestinationHostName( destinationHostName ),
                                            mSecondaryTcpConnectionDelay( AseTimeVal::INFINITE ),
                                            mFastTimeoutDelay( AseTimeVal::INFINITE ),
                                            mFastTimeoutMaxCount( 0 ),
                                            mHttpConnectionTimeout( AseTimeVal::INFINITE ),
                                            mHttpStallTimeout( AseTimeVal::INFINITE ),
                                            mSocketReceiveBufferSize( 0 ),
                                            mOutstandingEvent( NO_EVENT ),
                                            mPreviousState( LOOKING_UP_STATE ),
                                            mState( LOOKING_UP_STATE ),
                                            mStateEnterTime( AseTimeVal::now() ),
                                            mStartTime( AseTimeVal::now() ),
                                            mConnectingTime( AseTimeVal::INFINITE ),
                                            mConnectedTime( AseTimeVal::INFINITE ),
                                            mFailedTime( AseTimeVal::INFINITE ),
                                            mSocketReceiveEmptyTime( AseTimeVal::INFINITE ),
                                            mUserDataPtr( NULL ),
                                            mAuxUserDataPtr( NULL ),
                                            mConnectionFailureCode( 0 ),
                                            mLowLevelFailureCode( 0 ),
                                            mSocketHandle( -1 ),
                                            mSendingPtr( NULL ),
                                            mSendingDataLength( 0 ),
                                            mProcessingPtr( NULL ),
                                            mReceivedDataLength( 0 ),
                                            mItemPtr( NULL ),
                                            mRequestIdPool( 0 ),
                                            mReceivingPausedFlag( false ),
                                            mTraceListenerPtr( pTraceListener ),
                                            mIgnoredDataLength( 0 ),
                                            mFirstReceiveAfterResume( false ),
                                            mPreviousUnreadByteCount( 0 ),
                                            mSocketReattemptMaxDelay( AseTimeVal::INFINITE ),
                                            mMinimumRecvMode( false ),
                                            mConnectionReuse(false),
                                            mMaxContentLength(0LL)
{
    NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection created %p %s", this, destinationHostName.c_str());
    mSendingRequestItr = mRequestList.end();
    mReceivingRequestItr = mRequestList.end();

    for( int i = 0; i < ASYNC_IP_VERSION_COUNT; ++i )
    {
        mDestinationIpAddressItr[ i ] = mDestinationIpAddressList[ i ].end();
        mSocketReattemptTime[ i ] = AseTimeVal::INFINITE;
        mSocketReattemptDelay[ i ] = AseTimeVal::INFINITE;
        mSocketReattemptMaxDelay = AseTimeVal::INFINITE;
        mSocketReattemptSequence[ i ] = 0;
        mLastFailureCode[ i ] = AS_NO_ERROR;
        mLastLowLevelFailureCode[ i ] = AS_NO_ERROR;
    }
}

/** Destructor. */
AsyncHttpSocketConnection::~AsyncHttpSocketConnection()
{
    NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection died %p %s %d %llu",
        this, mDestinationHostName.c_str(),
        mSocketHandle, static_cast<ullong>((uintptr_t)mAuxUserDataPtr));
    close();
}

void AsyncHttpSocketConnection::setAsyncHttpSocketClient(AsyncHttpSocketClient* pAsyncHttpSocketClient)
{
    mAsyncHttpSocketClientPtr = pAsyncHttpSocketClient;
}

void AsyncHttpSocketConnection::setConnectionTraceListener(IHttpConnectionTraceListener* pTraceListener)
{
    mTraceListenerPtr = pTraceListener;
}

/** Set the IP connectivity policy. */
void AsyncHttpSocketConnection::setIpConnectivityPolicy(
                                    IpConnectivityManager::Policy ipConnectivityPolicy )
{
    mIpConnectivityPolicy = ipConnectivityPolicy;

    if ((mIpConnectivityPolicy == IpConnectivityManager::IP_V4_ONLY)
        || (mIpConnectivityPolicy == IpConnectivityManager::IP_V6_ONLY))
    {
        //IPv4 or IPv6 only, only one DNS lookup.
        mPendingDnsLookupCount = 1;
    }
    else
    {
        //Dual stack, two DNS lookups.
        mPendingDnsLookupCount = 2;
    }
}

/** Notify the DNS lookup result. */
int32_t AsyncHttpSocketConnection::notifyDnsLookupResult(
                                        const AseTimeVal& timeNow,
                                        int32_t statusCode,
                                        const NrdIpAddr& destinationIpAddr,
                                        const std::string &cname )
{
    std::vector<NrdIpAddr> addrList;

    if (statusCode == AS_NO_ERROR)
    {
        addrList.push_back( destinationIpAddr );
    }

    return notifyDnsLookupResult( timeNow, statusCode, addrList, statusCode == AS_NO_ERROR ? cname : std::string() );
}

/** Notify the DNS lookup result. */
int32_t AsyncHttpSocketConnection::notifyDnsLookupResult(
                                        const AseTimeVal& timeNow,
                                        int32_t statusCode,
                                        const std::vector<NrdIpAddr>& destinationIpAddrList,
                                        const std::string &cname)
{
    (void)cname;
#ifdef NRDP_HAS_TRACING
    std::ostringstream str;
    for (std::vector<NrdIpAddr>::const_iterator it = destinationIpAddrList.begin(); it != destinationIpAddrList.end(); ++it) {
        if (it != destinationIpAddrList.begin())
            str << ", ";
        str << it->getStrAddr();
    }

    NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p notifyDnsLookupResult status %d %s pending %d cname %s", this, statusCode, str.str().c_str(),
           mPendingDnsLookupCount, cname.c_str());
#endif
    int32_t retVal = AS_IN_PROGRESS;

    if (statusCode == AS_NO_ERROR)
    {
        assert( !destinationIpAddrList.empty() );

        mCName = cname;
        if (destinationIpAddrList.front().getVersion() == NrdIpAddr::IP_V4)
        {
            mDestinationIpAddressList[ASYNC_IP_V4_INDEX] = destinationIpAddrList;
        }
#ifdef NRDP_HAS_IPV6
        else
        {
            mDestinationIpAddressList[ASYNC_IP_V6_INDEX] = destinationIpAddrList;
        }
#endif
    }

    mPendingDnsLookupCount--;

    if (mPendingDnsLookupCount == 0)
    {
        //All DNS lookups are done, check if there are valid IP addresses for the
        //IP connectivty policy.

        retVal = statusCode;

#ifdef NRDP_HAS_IPV6
        retVal = checkConnectivityPolicy() ? AS_NO_ERROR : ( retVal == AS_NO_ERROR ) ? AS_DNS_ERROR : retVal;
#else
        retVal = mDestinationIpAddressList[ASYNC_IP_V4_INDEX].empty() ? AS_DNS_ERROR : retVal;
#endif

        if (retVal == AS_NO_ERROR)
        {
            retVal = open( timeNow );
        }
        else
        {
            mConnectionFailureCode = retVal;

            mFailedTime = timeNow;

            transit( FAILED_STATE );

            mOutstandingEvent = STATE_CHANGE_EVENT;

            if (mTraceListenerPtr)
            {
                mTraceListenerPtr->reportHttpConnectFailure(
                                                AS_DNS_ERROR,
                                                mDestinationHostName );
            }
        }
    }

    return retVal;
}

/** Set the destination port number. */
void AsyncHttpSocketConnection::setDestinationPortNumber( uint16_t destinationPortNumber )
{
    mDestinationPortNumber = destinationPortNumber;
}

/** Get the destination port number. */
uint16_t AsyncHttpSocketConnection::getDestinationPortNumber() const
{
    return mDestinationPortNumber;
}

/** Set the secondary TCP connection delay. */
void AsyncHttpSocketConnection::setSecondaryTcpConnectionDelay( const AseTimeVal& delay )
{
    mSecondaryTcpConnectionDelay = delay;
}

/** Set the fast TCP connection timeout delay. */
void AsyncHttpSocketConnection::setFastTcpTimeoutDelay( const AseTimeVal& delay )
{
    mFastTimeoutDelay = delay;
}

/** Set the fast TCP connection timeout max count. */
void AsyncHttpSocketConnection::setFastTcpTimeoutMaxCount( uint32_t maxCount )
{
    mFastTimeoutMaxCount = maxCount;
}

/** Set the HTTP connection timeout value. */
void AsyncHttpSocketConnection::setHttpConnectionTimeout( const AseTimeVal& timeout )
{
    mHttpConnectionTimeout = timeout;
}

/** Set the HTTP stall timeout value. */
void AsyncHttpSocketConnection::setHttpStallTimeout( const AseTimeVal& timeout )
{
    mHttpStallTimeout = timeout;
}

/** Set the socket receive buffer size. */
void AsyncHttpSocketConnection::setSocketReceiveBufferSize( uint32_t bufferSize )
{
    mSocketReceiveBufferSize = bufferSize;
}

/** Set the socket reattempt timeouts */
void AsyncHttpSocketConnection::setSocketReattemptDelays( AseTimeVal const& initial, AseTimeVal const& max )
{
    //NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p setSocketReattemptDelays( %llums, %llums )", this, initial.ms(), max.ms() );

    for( int i = 0; i < ASYNC_IP_VERSION_COUNT; ++i )
        mSocketReattemptDelay[ i ] = initial;

    mSocketReattemptMaxDelay = max;
}

bool AsyncHttpSocketConnection::shouldWaitForNextSocket( NrdIpAddr::Version protocol )
{
    int const protocolIndex = ( protocol == NrdIpAddr::IP_V6 ? ASYNC_IP_V6_INDEX : ASYNC_IP_V4_INDEX );

    std::vector<NrdIpAddr>& addressList = mDestinationIpAddressList[ protocolIndex ];
    std::vector<NrdIpAddr>::iterator& it = mDestinationIpAddressItr[ protocolIndex ];

    return !addressList.empty() && ( it == addressList.end() );
}

int32_t AsyncHttpSocketConnection::openNextSocket(  NrdIpAddr::Version protocol,
                                                    uint32_t sequence,
                                                    AseTimeVal const& fastTimeoutDelay )
{
    int32_t retVal = AS_CONNECTION_NO_ADDRESS;

    int const protocolIndex = ( protocol == NrdIpAddr::IP_V6 ? ASYNC_IP_V6_INDEX : ASYNC_IP_V4_INDEX );

    std::vector<NrdIpAddr>& addressList = mDestinationIpAddressList[ protocolIndex ];
    std::vector<NrdIpAddr>::iterator& it = mDestinationIpAddressItr[ protocolIndex ];

    if ( it == addressList.end() )
        it = addressList.begin();


    while( !addressList.empty() )
    {
        SocketAttemptPtr pSocketAttempt;

#ifdef NRDP_HAS_IPV6
        if ( it->getVersion() == NrdIpAddr::IP_V6 )
        {
            pSocketAttempt.reset( new SocketAttemptIpv6( this ) );
        }
        else
        {
            pSocketAttempt.reset( new SocketAttemptIpv4( this ) );
        }
#else
        pSocketAttempt.reset( new SocketAttemptIpv4( this ) );
#endif

        if ( !pSocketAttempt )
            return AS_NOT_ENOUGH_MEMORY;

        retVal = pSocketAttempt->open(  mAsyncHttpSocketClientPtr->peekConnectionId(),
                                        mHttpId,
                                        it->getStrAddr(),
                                        mDestinationPortNumber,
                                        sequence,
                                        fastTimeoutDelay,
                                        mSocketReceiveBufferSize,
                                        mConnectingTime - mStartTime );


        if ((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS))
        {
            if ( retVal == AS_IN_PROGRESS && mLastFailureCode[ protocolIndex ] == AS_NO_ERROR )
            {
                mLastFailureCode[ protocolIndex ] = AS_IN_PROGRESS;
            }

            (void) mAsyncHttpSocketClientPtr->allocateConnectionId();

            mSocketAttemptList.push_back( pSocketAttempt );

            if ( retVal == AS_NO_ERROR )
            {
                socketConnected( pSocketAttempt );
            }

            ++it;

            break;
        }
        else
        {
            mLastFailureCode[ protocolIndex ] = retVal;
            mLastLowLevelFailureCode[ protocolIndex ] = pSocketAttempt->getLowLevelFailureCode();
        }

        it = addressList.erase( it );

        if ( it == addressList.end() )
            it = addressList.begin();
    }

    return retVal;
}

bool AsyncHttpSocketConnection::checkConnectivityPolicy()
{
#ifdef NRDP_HAS_IPV6

    if ( mDestinationIpAddressList[ ASYNC_IP_V4_INDEX ].empty()
         && mDestinationIpAddressList[ ASYNC_IP_V6_INDEX ].empty() )
        return false;

    if ( mDestinationIpAddressList[ ASYNC_IP_V4_INDEX ].empty() )
    {
        if ( mIpConnectivityPolicy == IpConnectivityManager::IP_V4_ONLY )
            return false;
        mIpConnectivityPolicy = IpConnectivityManager::IP_V6_ONLY;
    }

    if ( mDestinationIpAddressList[ ASYNC_IP_V6_INDEX ].empty() )
    {
        if ( mIpConnectivityPolicy == IpConnectivityManager::IP_V6_ONLY )
            return false;
        mIpConnectivityPolicy = IpConnectivityManager::IP_V4_ONLY;
    }

#else
   if ( mDestinationIpAddressList[ ASYNC_IP_V4_INDEX ].empty() )
            return false;
   mIpConnectivityPolicy = IpConnectivityManager::IP_V4_ONLY;
#endif

    return true;
}

void AsyncHttpSocketConnection::socketConnected( SocketAttemptPtr pSocketAttempt )
{
    assert( mState == OPENING_STATE );
    assert( pSocketAttempt->connected() );

    mId = pSocketAttempt->getId();
    mSocketHandle = pSocketAttempt->transferSocketHandle();
    mActualSourceIpAddress = pSocketAttempt->getSourceIpAddress();
    mActualSourcePortNumber = pSocketAttempt->getSourcePort();
    mActualDestinationIpAddress = pSocketAttempt->getDestinationIpAddress();
    mActualSocketReceiveBufferSize = pSocketAttempt->getSocketReceiveBufferSize();
    mConnectedTime = pSocketAttempt->getTcpConnectedTime();

    mAsyncHttpSocketClientPtr->traceRouteIpAddress( mActualDestinationIpAddress );

    mSocketAttemptList.clear();
    mFrontSocketAttempt.reset();

    //Initialize various variables.
    mSendingRequestItr = mRequestList.end();

    mSendingPtr = mSendingBuffer;
    mSendingDataLength = 0;

    mProcessingPtr = mReceivingBuffer;
    mReceivedDataLength = 0;

    mIgnoredDataLength = mActualSocketReceiveBufferSize;
    mSocketReceiveEmptyTime = AseTimeVal::now();

    mOutstandingEvent = STATE_CHANGE_EVENT;

    mTimerOne = AseTimeVal::INFINITE;

    transit( IDLE_STATE );
}

void AsyncHttpSocketConnection::socketFailed( SocketAttemptPtr pSocketAttempt )
{
    int32_t const failureCode = pSocketAttempt->getFailureCode();

    if ( failureCode == AS_CONNECTION_REFUSED )
    {
        // Renove this address from our address list

        int const protocolIndex = ( pSocketAttempt->getProtocolVersion() == NrdIpAddr::IP_V6 ? ASYNC_IP_V6_INDEX : ASYNC_IP_V4_INDEX );

        std::vector<NrdIpAddr>& addressList = mDestinationIpAddressList[ protocolIndex ];
        std::vector<NrdIpAddr>::iterator& it = mDestinationIpAddressItr[ protocolIndex ];

        if ( !addressList.empty() )
        {
            NrdIpAddr const& destinationAddr = pSocketAttempt->getDestinationIpAddress();
            bool const haveNextAddress = ( it != addressList.end() );

            if ( haveNextAddress && (*it) == destinationAddr )
            {
                it = addressList.erase( it );
            }
            else
            {
                NrdIpAddr nextAddr;
                if ( haveNextAddress ) nextAddr = (*it);

                std::vector<NrdIpAddr>::iterator const failed = std::find( addressList.begin(), addressList.end(), destinationAddr );
                if ( failed != addressList.end() )
                {
                    addressList.erase( failed );

                    if ( haveNextAddress )
                        it = std::find( addressList.begin(), addressList.end(), nextAddr );
                    else
                        it = addressList.end();
                }
            }
        }
    }
}

/** Open the TCP connection. */
int32_t AsyncHttpSocketConnection::open( const AseTimeVal& timeNow )
{
    int32_t retVal = AS_CONNECTION_NO_ADDRESS;

    mConnectingTime = timeNow;

    mTimerOne = timeNow + mHttpConnectionTimeout;

    mDestinationIpAddressItr[ ASYNC_IP_V4_INDEX ] = mDestinationIpAddressList[ ASYNC_IP_V4_INDEX ].begin();
#ifdef NRDP_HAS_IPV6
    mDestinationIpAddressItr[ ASYNC_IP_V6_INDEX ] = mDestinationIpAddressList[ ASYNC_IP_V6_INDEX ].begin();
#endif

    NrdIpAddr::Version primaryVersion = NrdIpAddr::IP_V4;

    while( checkConnectivityPolicy() )
    {
        primaryVersion =
            ( ( mIpConnectivityPolicy == IpConnectivityManager::IP_V6_ONLY )
              || ( mIpConnectivityPolicy == IpConnectivityManager::IP_V6_V4 ) ) ? NrdIpAddr::IP_V6 : NrdIpAddr::IP_V4;

        retVal = openNextSocket( primaryVersion, 0, mFastTimeoutDelay );

        if (retVal == AS_NO_ERROR)
        {
            //The connection is established
            break;
        }
        else if (retVal == AS_IN_PROGRESS)
        {
            //The connection is in progress.
#ifdef NRDP_HAS_IPV6
            if ( ( mIpConnectivityPolicy == IpConnectivityManager::IP_V6_V4 )
                                || ( mIpConnectivityPolicy == IpConnectivityManager::IP_V4_V6 ) )
            {
                mSecondaryTcpConnectTime = timeNow + mSecondaryTcpConnectionDelay;
            }
            else
            {
                mSecondaryTcpConnectTime = AseTimeVal::INFINITE;
            }
#endif

            transit( OPENING_STATE );

            break;
        }
    }

    if ( retVal != AS_NO_ERROR && retVal != AS_IN_PROGRESS )
    {
        //The connection is failed.
        mOutstandingEvent = STATE_CHANGE_EVENT;

        mConnectionFailureCode = retVal;

        if ( retVal != AS_CONNECTION_NO_ADDRESS )
        {
            int const protocolIndex = ( primaryVersion == NrdIpAddr::IP_V6 ? ASYNC_IP_V6_INDEX : ASYNC_IP_V4_INDEX );

            mLowLevelFailureCode = mLastLowLevelFailureCode[ protocolIndex ];
        }

        transit( FAILED_STATE );

        if (mTraceListenerPtr)
        {
            mTraceListenerPtr->reportHttpConnectFailure(
                                    mConnectionFailureCode,
                                    mDestinationHostName );
        }
    }

    NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p Opening a new connection %s:%d retVal %d state %s", this,
           mDestinationHostName.c_str(), mDestinationPortNumber, retVal, stateNames[mState]);

    return retVal;
}

/** Close the TCP connection. */
int32_t AsyncHttpSocketConnection::close( int32_t reason )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    // All the open sockets are on the socket attempt list. They close when deleted.
    mSocketAttemptList.clear();

    if ( mSocketHandle != -1 )
    {
        NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p closing %d", this, mSocketHandle );

        ::close( mSocketHandle );

        mSocketHandle = -1;

        if ( mTraceListenerPtr )
            mTraceListenerPtr->reportTcpDisconnect(
                                            mId,
                                            mHttpId,
                                            reason,
                                            0,
                                            TCP_DISCONNECT_BY_LOCAL,
                                            false );
    }

    transit( CLOSED_STATE );

    retVal = AS_NO_ERROR;

    return retVal;
}

/** Get the socket handle. */
int AsyncHttpSocketConnection::getSocketHandle() const
{
    return mSocketHandle;
}

void AsyncHttpSocketConnection::setOutstandingEvent(Event event)
{
    mOutstandingEvent = event;
}

/** Check if there is any outstanding event. */
bool AsyncHttpSocketConnection::hasOutstandingEvent()
{
    bool retVal = false;

    if (mOutstandingEvent != NO_EVENT)
    {
        retVal = true;
    }
    else
    {
        //check the front of the request list for request that is completed or with response body
        //in the staging buffer.
        if (!mRequestList.empty())
        {
            if (mRequestList.front()->getState() == IAsyncHttpRequest::COMPLETED_STATE)
            {
                if (!mReceivingPausedFlag)
                {
                    retVal = true;
                }
            }
            else
            {
                if (mRequestList.front()->getState() == IAsyncHttpRequest::RECEIVING_BODY_STATE)
                {
                    if (mRequestList.front()->hasStagedResponseBody())
                    {
                        if (!mReceivingPausedFlag)
                        {
                            retVal = true;
                        }
                    }
                }
            }

            if (retVal)
            {
                mOutstandingEvent = RESPONSE_AVAILABLE_EVENT;
            }
        }
    }

    return retVal;
}

/** Signal the connection failure
 *
 *  This function is called from AsyncHttpSocketRequest when a receive operation fails
 */
void AsyncHttpSocketConnection::signalConnectionFailure(
                                    int32_t connectionFailureCode,
                                    int32_t lowLevelFailureCode )
{
    mConnectionFailureCode = connectionFailureCode;
    mLowLevelFailureCode = lowLevelFailureCode;

    transit( FAILED_STATE );

    mOutstandingEvent = STATE_CHANGE_EVENT;

    mSocketAttemptList.clear();

    ::close( mSocketHandle );

    mSocketHandle = -1;

    if (mTraceListenerPtr)
    {
        mTraceListenerPtr->reportTcpDisconnect(
            mId,
            mHttpId,
            mConnectionFailureCode,
            mLowLevelFailureCode,
            TCP_DISCONNECT_BY_REMOTE_WHILE_RECEIVING,
            (mReceivingRequestItr != mRequestList.end())
             && ((*mReceivingRequestItr)->getState() > IAsyncHttpRequest::WAITING_STATE) );
    }
}

/** Signal response body completed. */
void AsyncHttpSocketConnection::signalResponseBodyCompleted( AsyncHttpSocketRequest* pRequest )
{
#ifndef BUILD_DEBUG
    NRDP_UNUSED(pRequest);
#endif
    assert( pRequest == mRequestList.front().get() );

    if (mReceivingRequestItr == mRequestList.begin())
    {
        setupNextReceivingRequest();
    }

    mRequestList.pop_front();

    if (mRequestList.empty())
    {
        transit( IDLE_STATE );
    }
}

/** Reset the receiving buffer. */
void AsyncHttpSocketConnection::resetReceivingBuffer()
{
    if (mItemPtr)
    {
        uint32_t copiedDataLength;

        copiedDataLength = mReceivedDataLength - (mItemPtr - mReceivingBuffer);

        memmove(
            mReceivingBuffer,
            mItemPtr,
            copiedDataLength );

        mItemPtr = mReceivingBuffer;
        mProcessingPtr = mReceivingBuffer + copiedDataLength;
        mReceivedDataLength = copiedDataLength;
    }
    else
    {
        mProcessingPtr = mReceivingBuffer;
        mReceivedDataLength = 0;
    }
}

/** Send out requests. */
int32_t AsyncHttpSocketConnection::sendRequests( const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    while (mSendingRequestItr != mRequestList.end())
    {
        //Send pending data is there is any.
        if (mSendingDataLength > 0)
        {
            retVal = send(
                        mSocketHandle,
                        mSendingPtr,
                        mSendingDataLength,
                        0);

            if ((uint32_t) retVal == mSendingDataLength)
            {
                mSendingDataLength -= retVal;
            }
            else if ((retVal >= 0) && ((uint32_t) retVal < mSendingDataLength))
            {
                //Partial data is sent out.
                mSendingPtr += retVal;
                mSendingDataLength -= retVal;

                retVal = AS_IN_PROGRESS;

                break;
            }
            else if (retVal == -1)
            {
                //No data is send out or connection error.
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                {
                    retVal = AS_IN_PROGRESS;
                }
                else
                {
                    //Conne ction error, set connection failure code.
                    transit( FAILED_STATE );

                    mOutstandingEvent = STATE_CHANGE_EVENT;

                    mLowLevelFailureCode = errno;

                    if (errno == ECONNRESET)
                    {
                        retVal = AS_CONNECTION_RESET_WHILE_RECEIVING;
                    }
                    else
                    {
                        retVal = mapErrno( errno );
                    }

                    mConnectionFailureCode = retVal;

                    ::close( mSocketHandle );

                    mSocketHandle = -1;

                    if (mTraceListenerPtr)
                    {
                        mTraceListenerPtr->reportTcpDisconnect(
                            mId,
                            mHttpId,
                            mConnectionFailureCode,
                            mLowLevelFailureCode,
                            TCP_DISCONNECT_BY_REMOTE_WHILE_SENDING,
                            (mReceivingRequestItr != mRequestList.end())
                             && ((*mReceivingRequestItr)->getState() > IAsyncHttpRequest::WAITING_STATE) );
                    }
                }

                break;
            }
        }

        retVal = AS_NO_ERROR;

        if ((*mSendingRequestItr)->getState() == IAsyncHttpRequest::CREATED_STATE)
        {
            (*mSendingRequestItr)->setTimeoutTime( timeNow );
        }

        if (((*mSendingRequestItr)->getState() == IAsyncHttpRequest::CREATED_STATE)
            || ((*mSendingRequestItr)->getState() == IAsyncHttpRequest::SENDING_LINE_HEADERS_STATE))
        {
            //Start encoding the request to the sending staging buffer.
            mSendingDataLength = SENDING_STAGING_BUFFER_SIZE;

            (*mSendingRequestItr)->encodeRequest(
                                            mSendingBuffer,
                                            mSendingDataLength );

            if (mSendingDataLength == 0)
            {
                (*mSendingRequestItr)->getRequestBody(
                                            mSendingPtr,
                                            mSendingDataLength );

                if (mSendingDataLength > 0)
                {
                    (*mSendingRequestItr)->setState( IAsyncHttpRequest::SENDING_BODY_STATE );
                }
                else
                {
                    (*mSendingRequestItr)->setState( IAsyncHttpRequest::WAITING_STATE );

                    (*mSendingRequestItr)->setTimeoutTime( timeNow );

                    if (mReceivingRequestItr == mRequestList.end())
                    {
                        mReceivingRequestItr = mSendingRequestItr;

                        //Set the stall timeout time.
                        mTimerOne = timeNow + mHttpStallTimeout;

                        //Set the socket empty time
                        mSocketReceiveEmptyTime = timeNow;
                    }

                    mSendingRequestItr++;
                }
            }
            else
            {
                mSendingPtr = mSendingBuffer;
            }
        }
        else
        {
            assert( ((*mSendingRequestItr)->getState() == IAsyncHttpRequest::SENDING_BODY_STATE) );

            (*mSendingRequestItr)->setState( IAsyncHttpRequest::WAITING_STATE );

            (*mSendingRequestItr)->setTimeoutTime( timeNow );

            if (mReceivingRequestItr == mRequestList.end())
            {
                mReceivingRequestItr = mSendingRequestItr;

                //Set the stall timeout time.
                mTimerOne = timeNow + mHttpStallTimeout;

                //Set the socket empty time
                mSocketReceiveEmptyTime = timeNow;
            }

            mSendingRequestItr++;
        }
    }

    return retVal;
}

/** Prepare the data for calling select(). */
void AsyncHttpSocketConnection::prepareSelect(
                                    const AseTimeVal& timeNow,
                                    int& selectingFd,
                                    fd_set* pReadFdSet,
                                    fd_set* pWriteFdSet,
                                    AseTimeVal& timeout )
{
    selectingFd = -1;
    timeout = AseTimeVal::INFINITE;

    switch (mState)
    {
        case OPENING_STATE:
        {
            std::list<SocketAttemptPtr>::const_iterator it = mSocketAttemptList.begin();
            for( ; it != mSocketAttemptList.end(); ++it )
            {
                int const fd = (*it)->getOsSocketHandle();
                if ( fd != -1 )
                {
                    selectingFd = std::max( selectingFd, fd );
                    timeout = std::min( timeout, (*it)->getFastTimeout() );
                    FD_SET( fd, pWriteFdSet );
                }
                //NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p OPENING select socket %d", this, (*it)->getOsSocketHandle() );
            }
#ifdef NRDP_HAS_IPV6
            timeout = std::min( timeout, mSecondaryTcpConnectTime );
            timeout = std::min( timeout, mSocketReattemptTime[ ASYNC_IP_V6_INDEX ] );
#endif
            timeout = std::min( timeout, mSocketReattemptTime[ ASYNC_IP_V4_INDEX ] );
            timeout = std::min( timeout, mTimerOne );
            timeout = std::max( timeout, timeNow );

            if ( timeout != AseTimeVal::INFINITE )
                timeout -= timeNow;

            break;
        }

        case IDLE_STATE:
        {
            assert( mSocketHandle != -1 );

            selectingFd = mSocketHandle;

            FD_SET( mSocketHandle, pReadFdSet );
            //NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p IDLE select socket %d", this, mSocketHandle );

            break;
        }

        case ACTIVE_STATE:
        {
            assert( !mRequestList.empty() );
            assert( mSocketHandle != -1 );

            //Set up write FD.
            if (mSendingRequestItr != mRequestList.end())
            {
                selectingFd = mSocketHandle;

                FD_SET( mSocketHandle, pWriteFdSet );
                //NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p ACTIVE select socket %d (sending)", this, mSocketHandle );
            }

            //Check if the connection is paused on receiving.
            if (!mReceivingPausedFlag)
            {
                //Set up read FD.
                if (mReceivingRequestItr != mRequestList.end())
                {
                    assert( (*mReceivingRequestItr)->getState()
                                    >= IAsyncHttpRequest::WAITING_STATE );

                    if ((*mReceivingRequestItr)->getState() >= IAsyncHttpRequest::WAITING_STATE)
                    {
                        selectingFd = mSocketHandle;

                        FD_SET( mSocketHandle, pReadFdSet );
                        //NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p ACTIVE select socket %d (receiving)", this, mSocketHandle );
                    }
                }

                //Set up timeout
                AseTimeVal earlyTimeout = AseTimeVal::INFINITE;

                if (mReceivingRequestItr != mRequestList.end())
                {
                    earlyTimeout = (*mReceivingRequestItr)->getTimeoutTime();
                }
                else if (mSendingRequestItr != mRequestList.end())
                {
                    earlyTimeout = (*mSendingRequestItr)->getTimeoutTime();
                }

                if (earlyTimeout > mTimerOne)
                {
                    earlyTimeout = mTimerOne;
                }

                if (earlyTimeout > timeNow)
                {
                    timeout = earlyTimeout - timeNow;
                }
                else
                {
                    timeout = AseTimeVal::ZERO;
                }
            }
            else
            {
                timeout = AseTimeVal::INFINITE;
            }

            break;
        }

        case LOOKING_UP_STATE:
        case CLOSED_STATE:
        case FAILED_STATE:
        default:
        {
            break;
        }
    }

    return;
}

/** Process the select events. */
void AsyncHttpSocketConnection::processSelect(
                                    const AseTimeVal& timeNow,
                                    fd_set* pReadFdSet,
                                    fd_set* pWriteFdSet )
{
    switch (mState)
    {
        case OPENING_STATE:
        {
            int32_t retVal = AS_IN_PROGRESS;

            if ( !mSocketAttemptList.empty() )
                mFrontSocketAttempt = mSocketAttemptList.front();

            // First check whether any current attempts have succeeded or failed
            std::list<SocketAttemptPtr>::iterator it = mSocketAttemptList.begin();
            while( it != mSocketAttemptList.end() )
            {
                if ( FD_ISSET( (*it)->getOsSocketHandle(), pWriteFdSet ) )
                {
                    FD_CLR( (*it)->getOsSocketHandle(), pWriteFdSet );

                    (*it)->processSelect( timeNow );
                    if ( (*it)->connected() )
                    {
                        socketConnected( *it );
                        retVal = AS_NO_ERROR;
                        break;
                    }
                    else if ( (*it)->failed() )
                    {
                        int const protocolIndex = ( (*it)->getProtocolVersion() == NrdIpAddr::IP_V6 ? ASYNC_IP_V6_INDEX : ASYNC_IP_V4_INDEX );

                        mLastFailureCode[ protocolIndex ] = (*it)->getFailureCode();
                        mLastLowLevelFailureCode[ protocolIndex ] = (*it)->getLowLevelFailureCode();

                        socketFailed( (*it) );

                        // Check if we should start another another attempt
                        if ( shouldWaitForNextSocket( (*it)->getProtocolVersion() ) )
                        {

                            //NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p socket %d failed - setting timer for %llums", this,
                            //    (*it)->getOsSocketHandle(),
                            //    mSocketReattemptDelay[ protocolIndex ].ms() );

                            if ( mSocketReattemptTime[ protocolIndex ] == AseTimeVal::INFINITE
                                    && mSocketReattemptDelay[ protocolIndex ] != AseTimeVal::INFINITE )
                            {
                                mSocketReattemptTime[ protocolIndex ] = timeNow + mSocketReattemptDelay[ protocolIndex ];

                                mSocketReattemptDelay[ protocolIndex ] = std::min( mSocketReattemptDelay[ protocolIndex ] * 2,
                                                                                    mSocketReattemptMaxDelay );

                                mSocketReattemptSequence[ protocolIndex ] = (*it)->getSequence();
                            }

                            retVal = AS_IN_PROGRESS;
                        }
                        else
                        {
                            retVal = openNextSocket( (*it)->getProtocolVersion(), 0, mFastTimeoutDelay );
                            if (retVal == AS_NO_ERROR)
                                break;
                        }

                        it = mSocketAttemptList.erase( it );
                        continue;
                    }
                }

                ++it;
            }

            if ( retVal == AS_NO_ERROR )
            {
                // Socket connected
            }
            else if ( retVal == AS_IN_PROGRESS )
            {
                // Now check for the overall timeout
                if ( timeNow > mTimerOne )
                {
                    mFailedTime = timeNow;

#ifdef NRDP_HAS_IPV6
                    // We report this as a timeout if wwe actually tried to open and it never failed on one protocol or the other
                    if ( mLastFailureCode[ ASYNC_IP_V4_INDEX ] == AS_IN_PROGRESS
                        || mLastFailureCode[ ASYNC_IP_V6_INDEX ] == AS_IN_PROGRESS )
                    {
                        mConnectionFailureCode = AS_HTTP_CONNECTION_TIMEOUT;
                        mLowLevelFailureCode = 0;
                    }
                    else
                    {
                        // FIXME: If both IPv4 and IPv6 fail we should report the error for the first one we attempted
                        //        Here we report IPv4 if both failed
                        int const protocolIndex = ( mLastFailureCode[ ASYNC_IP_V4_INDEX ] == AS_NO_ERROR ) ? ASYNC_IP_V6_INDEX : ASYNC_IP_V4_INDEX;

                        mConnectionFailureCode = mLastFailureCode[ protocolIndex ];
                        mLowLevelFailureCode = mLastLowLevelFailureCode[ protocolIndex ];
                    }
#else
                    if ( mLastFailureCode[ ASYNC_IP_V4_INDEX ] == AS_IN_PROGRESS )
                    {
                        mConnectionFailureCode = AS_HTTP_CONNECTION_TIMEOUT;
                        mLowLevelFailureCode = 0;
                    }
                    else
                    {
                        mConnectionFailureCode = mLastFailureCode[ ASYNC_IP_V4_INDEX ];
                        mLowLevelFailureCode = mLastLowLevelFailureCode[ ASYNC_IP_V4_INDEX ];
                    }
#endif

                    while ( !mSocketAttemptList.empty() )
                    {
                        mSocketAttemptList.front()->close( AS_HTTP_CONNECTION_TIMEOUT );
                        mSocketAttemptList.pop_front();
                    }

                    if (mTraceListenerPtr)
                    {
                        mTraceListenerPtr->reportHttpConnectFailure(
                                                    mConnectionFailureCode,
                                                    mDestinationHostName );
                    }

                    mFrontSocketAttempt.reset();

                    mOutstandingEvent = STATE_CHANGE_EVENT;

                    transit( FAILED_STATE );
                }
                else
                {
#ifdef NRDP_HAS_IPV6
                    if ( timeNow > mSecondaryTcpConnectTime )
                    {
                        // Secondary protocol timeout
                        NrdIpAddr::Version const secondaryVersion =
                            ( mIpConnectivityPolicy == IpConnectivityManager::IP_V6_V4 ) ? NrdIpAddr::IP_V4 : NrdIpAddr::IP_V6;

                        retVal = openNextSocket( secondaryVersion, 0, mFastTimeoutDelay );

                        mSecondaryTcpConnectTime = AseTimeVal::INFINITE;
                    }

                    if ( retVal == AS_IN_PROGRESS && timeNow > mSocketReattemptTime[ ASYNC_IP_V6_INDEX ] )
                    {
                        // IPv6 re-attempt
                        mSocketReattemptTime[ ASYNC_IP_V6_INDEX ] = AseTimeVal::INFINITE;
                        retVal = openNextSocket(    NrdIpAddr::IP_V6,
                                                    mSocketReattemptSequence[ ASYNC_IP_V6_INDEX ],
                                                    mFastTimeoutDelay * ( 1 << mSocketReattemptSequence[ ASYNC_IP_V6_INDEX ] ) );
                    }
#endif
                    if ( retVal == AS_IN_PROGRESS && timeNow > mSocketReattemptTime[ ASYNC_IP_V4_INDEX ] )
                    {
                        // IPv4 re-attempt
                        mSocketReattemptTime[ ASYNC_IP_V4_INDEX ] = AseTimeVal::INFINITE;
                        retVal = openNextSocket(    NrdIpAddr::IP_V4,
                                                    mSocketReattemptSequence[ ASYNC_IP_V4_INDEX ],
                                                    mFastTimeoutDelay * ( 1 << mSocketReattemptSequence[ ASYNC_IP_V4_INDEX ] ) );
                    }

                    // Check for fast timeouts
                    if ( retVal == AS_IN_PROGRESS )
                    {
                        std::list<SocketAttemptPtr>::const_iterator it = mSocketAttemptList.begin();
                        for( ; it != mSocketAttemptList.end(); ++it )
                        {
                            if ( timeNow > (*it)->getFastTimeout() )
                            {
                                if ( (*it)->getSequence() < mFastTimeoutMaxCount )
                                {
                                    NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p %s fastTcpTimeout after %llums",
                                            this,
                                            mDestinationHostName.c_str(),
                                            (*it)->getFastTimeoutDelay().ms() );

                                    retVal = openNextSocket(    (*it)->getProtocolVersion(),
                                                                (*it)->getSequence() + 1,
                                                                (*it)->getFastTimeoutDelay() * 2 );
                                    if ( retVal == AS_NO_ERROR )
                                    {
                                        break;
                                    }
                                }

                                (*it)->clearFastTimeout();
                            }
                        }

                        // Ignore synchronous failues on fast timeouts - the original attempt is still running
                        if ( retVal != AS_NO_ERROR )
                        {
                            retVal = AS_IN_PROGRESS;
                        }
                    }
                }
            }

            if ( retVal != AS_NO_ERROR && retVal != AS_IN_PROGRESS && !checkConnectivityPolicy() )
            {
                //The connection has failed.
#ifdef NRDP_HAS_IPV6
                // FIXME: If both IPv4 and IPv6 fail we should report the error for the first one we attempted
                //        Here we report IPv4 if both failed
                if ( mLastFailureCode[ ASYNC_IP_V4_INDEX ] != AS_NO_ERROR )
                {
                    mConnectionFailureCode = mLastFailureCode[ ASYNC_IP_V4_INDEX ];
                    mLowLevelFailureCode = mLastLowLevelFailureCode[ ASYNC_IP_V4_INDEX ];
                }
                else
                {
                    mConnectionFailureCode = mLastFailureCode[ ASYNC_IP_V6_INDEX ];
                    mLowLevelFailureCode = mLastLowLevelFailureCode[ ASYNC_IP_V6_INDEX ];
                }
#else
                mConnectionFailureCode = mLastFailureCode[ ASYNC_IP_V4_INDEX ];
                mLowLevelFailureCode = mLastLowLevelFailureCode[ ASYNC_IP_V4_INDEX ];
#endif

                if (mTraceListenerPtr)
                {
                    mTraceListenerPtr->reportHttpConnectFailure(
                                            mConnectionFailureCode,
                                            mDestinationHostName );
                }

                mOutstandingEvent = STATE_CHANGE_EVENT;

                transit( FAILED_STATE );
            }

            break;
        }

        case IDLE_STATE:
        {
            // server closes the connection
            if (FD_ISSET( mSocketHandle, pReadFdSet ))
            {
                assert( mSocketHandle != -1 );

                int optVal;
                int optValLen;

                optValLen = sizeof( optVal );

                // Return any pending errors in the socket. The value returned
                // corresponds to the standard error codes defined in <errno.h>
                if (getsockopt(
                            mSocketHandle,
                            SOL_SOCKET,
                            SO_ERROR,
                            (void*) &optVal,
                            (socklen_t*) &optValLen ) == 0)
                {
                    mConnectionFailureCode = mapErrno( optVal );
                    mLowLevelFailureCode = optVal;
                }
                else
                {
                    mConnectionFailureCode = AS_SOCKET_ERROR;
                    mLowLevelFailureCode = 0;
                }

                transit( CLOSED_STATE );

                ::close( mSocketHandle );

                mSocketHandle = -1;

                if (mTraceListenerPtr)
                {
                    mTraceListenerPtr->reportTcpDisconnect(
                        mId,
                        mHttpId,
                        mConnectionFailureCode,
                        mLowLevelFailureCode,
                        TCP_DISCONNECT_BY_REMOTE_WHILE_IDLE,
                        false );
                }

                mOutstandingEvent = STATE_CHANGE_EVENT;
            }

            break;
        }

        case ACTIVE_STATE:
        {
            assert( !mRequestList.empty() );
            assert( mSocketHandle != -1 );

            int32_t retVal = AS_NO_ERROR;

            if (FD_ISSET( mSocketHandle, pWriteFdSet ))
            {
                retVal = sendRequests( timeNow );
            }

            if (!mReceivingPausedFlag)
            {
                //Make sure there is no connection error.
                if (((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS))
                    && (FD_ISSET( mSocketHandle, pReadFdSet)))
                {
                    //NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p ACTIVE process select socket %d", this, mSocketHandle );

                    receiveResponses();

                    mTimerOne = timeNow + mHttpStallTimeout;
                }
                else if ((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS))
                {

                    RequestList::iterator itrRequest = mRequestList.end();

                    if (mReceivingRequestItr != mRequestList.end())
                    {
                        itrRequest = mReceivingRequestItr;
                    }
                    else if (mSendingRequestItr != mRequestList.end())
                    {
                        itrRequest = mSendingRequestItr;
                    }

                    if ((itrRequest != mRequestList.end())
                        && (timeNow >= (*itrRequest)->getTimeoutTime()))
                    {
                        //Response timed out.
                        transit( FAILED_STATE );

                        mConnectionFailureCode = AS_HTTP_TRANSACTION_TIMEOUT;
                        mLowLevelFailureCode = 0;

                        ::close( mSocketHandle );

                        mSocketHandle = -1;

                        if (mTraceListenerPtr)
                        {
                            mTraceListenerPtr->reportTcpDisconnect(
                                mId,
                                mHttpId,
                                mConnectionFailureCode,
                                mLowLevelFailureCode,
                                TCP_DISCONNECT_BY_REMOTE_WHILE_RECEIVING,
                                (*itrRequest)->getState() > IAsyncHttpRequest::WAITING_STATE );
                        }

                        mOutstandingEvent = STATE_CHANGE_EVENT;

                    }
                    else if (timeNow >= mTimerOne)
                    {
                        //Stall timed out.
                        transit( FAILED_STATE );

                        mConnectionFailureCode = AS_HTTP_CONNECTION_STALL;
                        mLowLevelFailureCode = 0;

                        ::close( mSocketHandle );

                        mSocketHandle = -1;

                        if (mTraceListenerPtr)
                        {
                            mTraceListenerPtr->reportTcpDisconnect(
                                mId,
                                mHttpId,
                                mConnectionFailureCode,
                                mLowLevelFailureCode,
                                TCP_DISCONNECT_BY_REMOTE_WHILE_RECEIVING,
                                (mReceivingRequestItr != mRequestList.end())
                                 && ((*mReceivingRequestItr)->getState() > IAsyncHttpRequest::WAITING_STATE) );
                        }

                        mOutstandingEvent = STATE_CHANGE_EVENT;
                    }
                }
            }
            /*
            else
            {
                if (FD_ISSET( mSocketHandle, pReadFdSet))
                {
                    NTRACE(TRACE_HTTPLIB, "AsyncHttpSocketConnection %p ACTIVE process select paused socket %d", this, mSocketHandle );
                }
            }
            */

            break;
        }

        case LOOKING_UP_STATE:
        case CLOSED_STATE:
        case FAILED_STATE:
        default:
        {
            break;
        }
    }

    return;
}

/** Receive data from the socket. */
int32_t AsyncHttpSocketConnection::receive(unsigned char* pBuffer,
                                           uint32_t bufferLength,
                                           int32_t flags )
{
    int32_t retVal = 0;

    AseTimeVal const timeNow = AseTimeVal::now();

#ifdef HAVE_FIONREAD
    int32_t ioctlRetVal;
    int32_t unReadByteCount = -1;

// The following hides bytes in the buffer on resume from the layers above, but we should report them
#if 0
    if (mFirstReceiveAfterResume)
    {
        ioctlRetVal = ioctl(
            mSocketHandle,
            FIONREAD,
            &unReadByteCount );

        assert( ioctlRetVal == 0 );

        mPreviousUnreadByteCount = unReadByteCount;

        mFirstReceiveAfterResume = false;
    }
#endif
#endif

    if(mMinimumRecvMode) {
        flags |= MSG_PEEK;
    }

    if ( mAsyncHttpSocketClientPtr->trafficShaperWouldSleep() )
    {
        retVal = -1;
        errno = EWOULDBLOCK;
    }
    else
    {
        retVal = recv(  mSocketHandle,
                        pBuffer,
                        bufferLength,
                        flags );

        mAsyncHttpSocketClientPtr->trafficShaperConsume( retVal > 0 ? retVal : 0  );
    }

    if (retVal > 0)
    {
#ifdef HAVE_FIONREAD
        ioctlRetVal = ioctl(
            mSocketHandle,
            FIONREAD,
            &unReadByteCount );
        if (ioctlRetVal == 0) {
            assert(unReadByteCount >= 0);
            if (static_cast<uint32_t>(unReadByteCount + retVal) < mPreviousUnreadByteCount) {
                NTRACE(TRACE_HTTPLIB, "Unexpected socket bookkeeping for %p:%s, unReadByteCount: %d, retVal: %d, mPreviousUnreadByteCount: %d",
                       this, mActualDestinationIpAddress.getStrAddr().c_str(), unReadByteCount, retVal, mPreviousUnreadByteCount);
            }
        } else {
            NTRACE(TRACE_HTTPLIB, "ioctl failed: %s %d/%d/%d", mActualDestinationIpAddress.getStrAddr().c_str(),
                   mSocketHandle, errno, retVal);
            unReadByteCount = 0;
        }

        if (mTraceListenerPtr)
        {
            mTraceListenerPtr->reportBytesReceived(
                mId,
                timeNow,
                static_cast<uint32_t>(std::max(0, ((unReadByteCount + retVal) - static_cast<int>(mPreviousUnreadByteCount)))),
                retVal,
                mIgnoredDataLength < mActualSocketReceiveBufferSize,
                mSocketReceiveEmptyTime );
        }

        mPreviousUnreadByteCount = unReadByteCount;

        if ( unReadByteCount == retVal )
        {
            // We read all the bytes
            mIgnoredDataLength = mActualSocketReceiveBufferSize;
            mSocketReceiveEmptyTime = timeNow;
        }
#else
        if (mTraceListenerPtr)
        {
            mTraceListenerPtr->reportBytesReceived(
                mId,
                timeNow,
                0xFFFFFFFF,
                retVal,
                mIgnoredDataLength < mActualSocketReceiveBufferSize,
                mSocketReceiveEmptyTime );
        }

#endif

        if (mIgnoredDataLength < mActualSocketReceiveBufferSize)
        {
            mIgnoredDataLength += retVal;
        }
        if ( mIgnoredDataLength >= mActualSocketReceiveBufferSize )
        {
            mSocketReceiveEmptyTime = timeNow;
        }
    }
    else
    {
        // No more bytes in the socket buffer
        mIgnoredDataLength = mActualSocketReceiveBufferSize;
        mSocketReceiveEmptyTime = timeNow;
    }

    return retVal;
}

/** Get the TCP ID. */
uint32_t AsyncHttpSocketConnection::getId() const
{
    return mId;
}

/** Get the HTTP ID. */
uint32_t AsyncHttpSocketConnection::getHttpId() const
{
    return mHttpId;
}

/** Retrieve the outstanding event. */
IAsyncHttpConnection::Event AsyncHttpSocketConnection::retrieveOutstandingEvent()
{
    Event retEvent = mOutstandingEvent;

    mOutstandingEvent = NO_EVENT;

    return retEvent;
}

/** Get the current state */
IAsyncHttpConnection::State AsyncHttpSocketConnection::getState() const
{
    return mState;
}

/** Get the current state and how long the connection has been in the state. */
IAsyncHttpConnection::State AsyncHttpSocketConnection::getState( AseTimeVal& duration ) const
{
    duration = AseTimeVal::now() - mStateEnterTime;

    return mState;
}

bool AsyncHttpSocketConnection::didOpen() const
{
    return ( mConnectedTime != AseTimeVal::INFINITE );
}

/** Get the client IP address, only valid when state > LOOKING_UP_STATE and state < FAILED_STATE  */
NrdIpAddr AsyncHttpSocketConnection::getClientIpAddress() const
{
    return mActualSourceIpAddress;
}

/** Get the client port number, only valid when state > LOOKING_UP_STATE and state < FAILED_STATE  */
uint16_t AsyncHttpSocketConnection::getClientPortNumber() const
{
    return ntohs( mActualSourcePortNumber );
}

/** Get the server name. */
const std::string& AsyncHttpSocketConnection::getServerName() const
{
    return mDestinationHostName;
}

/** Get the resovled server IP address, only valid when state > LOOKING_UP_STATE and state < FAILED_STATE  */
NrdIpAddr AsyncHttpSocketConnection::getServerIpAddress() const
{
    return mActualDestinationIpAddress;
}

std::string AsyncHttpSocketConnection::getCName() const
{
    return mCName;
}

/** Get the secondary TCP connection delay. */
const AseTimeVal& AsyncHttpSocketConnection::getSecondaryTcpConnectionDelay() const
{
    return mSecondaryTcpConnectionDelay;
}

/** Get the fast TCP connection timeout delay. */
const AseTimeVal& AsyncHttpSocketConnection::getFastTcpTimeoutDelay() const
{
    return mFastTimeoutDelay;
}

/** Get the fast TCP connection timeout maximum socket count. */
uint32_t AsyncHttpSocketConnection::getFastTcpTimeoutMaxCount() const
{
    return mFastTimeoutMaxCount;
}

/** Get the HTTP connection timeout value. */
const AseTimeVal& AsyncHttpSocketConnection::getHttpConnectionTimeout() const
{
    return mHttpConnectionTimeout;
}

/** Get the HTTP stall timeout value. */
const AseTimeVal& AsyncHttpSocketConnection::getHttpStallTimeout() const
{
    return mHttpStallTimeout;
}

/** Get the socket receive buffer size. */
uint32_t AsyncHttpSocketConnection::getSocketReceiveBufferSize() const
{
    return mSocketReceiveBufferSize;
}

/** Get the actual socket receive buffer size. */
uint32_t AsyncHttpSocketConnection::getActualSocketReceiveBufferSize() const
{
    return mActualSocketReceiveBufferSize;
}

/** Get the time the client starts the connection. */
const AseTimeVal& AsyncHttpSocketConnection::getStartTime() const
{
    return mStartTime;
}

/** Get the time the client attempts to connect to the server. */
const AseTimeVal& AsyncHttpSocketConnection::getConnectingTime() const
{
    return mConnectingTime;
}

/** Get the time the client finishes connecting to the server. */
const AseTimeVal& AsyncHttpSocketConnection::getConnectedTime() const
{
    return mConnectedTime;
}

/** Get the time the client failes to connect to the server. */
const AseTimeVal& AsyncHttpSocketConnection::getFailedTime() const
{
    return mFailedTime;
}

/** Set the user data which is some opaque data which the caller wants to associate with the connection. */
void AsyncHttpSocketConnection::setUserData( void* pData )
{
    mUserDataPtr = pData;
}

/** Get the user data. */
void* AsyncHttpSocketConnection::getUserData() const
{
    return mUserDataPtr;
}

/** Set the auxiliary user data. */
void AsyncHttpSocketConnection::setAuxUserData( void* pData )
{
    mAuxUserDataPtr = pData;
}

/** Get the axuiliary user data. */
void* AsyncHttpSocketConnection::getAuxUserData() const
{
    return mAuxUserDataPtr;
}

/** Get the connection failure code */
int32_t AsyncHttpSocketConnection::getConnectionFailureCode() const
{
    return mConnectionFailureCode;
}

/** Get the low level failure code */
int32_t AsyncHttpSocketConnection::getLowLevelFailureCode() const
{
    return mLowLevelFailureCode;
}

void AsyncHttpSocketConnection::setMinimumRecvMode(bool enable)
{
    mMinimumRecvMode = enable;
}

/** Issue an HTTP get request. */
int32_t AsyncHttpSocketConnection::issueGetRequest(
                                        AseUrl const& url,
                                        HttpHeaders const& headers,
                                        IHttpRequestTraceListenerPtr pTraceListener,
                                        IAsyncHttpRequestPtr& pAsyncHttpRequest )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    retVal = issueRequest(
                    IAsyncHttpRequest::GET_METHOD,
                    IAsyncHttpRequest::HTTP_1_1,
                    url,
                    headers,
                    NULL,
                    0,
                    AseTimeVal::INFINITE,
                    pTraceListener,
                    pAsyncHttpRequest );

    return retVal;
}

/** Issue an HTTP request. */
int32_t AsyncHttpSocketConnection::issueRequest(
                                        IAsyncHttpRequest::Method method,
                                        IAsyncHttpRequest::Version version,
                                        AseUrl const& url,
                                        HttpHeaders const& headers,
                                        unsigned char* pBodyData,
                                        uint32_t bodyLength,
                                        const AseTimeVal& timeoutDuration,
                                        IHttpRequestTraceListenerPtr pTraceListener,
                                        IAsyncHttpRequestPtr& pAsyncHttpRequest )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    //Make sure the connection is open.
    if ((mState == IDLE_STATE)
        || (mState == ACTIVE_STATE))
    {
        /*
        if(mConnectionReuse)
            NTRACE(TRACE_HTTPLIB, "issueRequest %p", this);
        */

        AsyncHttpSocketRequest* pHttpSocketRequest;

        pHttpSocketRequest = new AsyncHttpSocketRequest(
                                                this,
                                                mRequestIdPool++,
                                                method,
                                                version,
                                                pTraceListener );

        if (pHttpSocketRequest)
        {
            pHttpSocketRequest->setMaxContentLength(mMaxContentLength);
            HttpHeaders requestHeaders;

            //Initialize this request.
            pHttpSocketRequest->setRequestUrl( url );

            //Set the request headers by merging the extra headers from the client
            //and headers passed in here.
            requestHeaders = mAsyncHttpSocketClientPtr->getExtraRequestHeaders();
            requestHeaders.insert( headers.begin(), headers.end() );

            pHttpSocketRequest->setRequestHeaders( requestHeaders );
            pHttpSocketRequest->setRequestBody( pBodyData, bodyLength );
            pHttpSocketRequest->setTimeoutDuration( timeoutDuration );

            AseTimeVal timeNow = AseTimeVal::now();

            if (mReceivingPausedFlag)
            {
                pHttpSocketRequest->pauseReceiving( timeNow );
            }

            //Check if there is any request being sent out.
            if (mSendingRequestItr == mRequestList.end())
            {
                //Add the request to the end of the list and get the iterator.
                AsyncHttpSocketRequestPtr tempPtr( pHttpSocketRequest );

                mSendingRequestItr = mRequestList.insert(
                                                mRequestList.end(),
                                                tempPtr );

                pAsyncHttpRequest = tempPtr;

                if ((mState == IDLE_STATE) || (mState == ACTIVE_STATE))
                {
                    //Send the request.
                    retVal = sendRequests( timeNow );

                    if ((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS))
                    {
                        if (mState == IDLE_STATE)
                        {

                            transit( ACTIVE_STATE );
                        }

                        retVal = AS_NO_ERROR;
                    }
                }
                else
                {
                    retVal = AS_NO_ERROR;
                }
            }
            else
            {
                //There are requests being sent out, add this request at the
                //end of the list.
                AsyncHttpSocketRequestPtr tempPtr( pHttpSocketRequest );

                mRequestList.push_back( tempPtr );

                pAsyncHttpRequest = tempPtr;

                retVal = AS_NO_ERROR;
            }
        }
        else
        {
            retVal = AS_NOT_ENOUGH_MEMORY;
        }
    }
    else
    {
        retVal = AS_CONNECTION_NOT_OPEN;
    }

    if (retVal != AS_NO_ERROR)
    {
        pAsyncHttpRequest.reset();
    }

    return retVal;
}


/** Cancel an HTTP request. */
int32_t AsyncHttpSocketConnection::cancelRequest(
                                       IAsyncHttpRequestPtr const& pAsyncHttpRequest )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    RequestList::iterator requestItr = mRequestList.begin();

    while (requestItr != mRequestList.end())
    {
        if ((*requestItr)->getId() == pAsyncHttpRequest->getId())
        {
            if ((*requestItr)->getState() == IAsyncHttpRequest::CREATED_STATE)
            {
                mRequestList.erase( requestItr );

                retVal = AS_NO_ERROR;
            }
            else
            {
                retVal = AS_IN_PROGRESS;
            }
        }

        requestItr++;
    }

    if (requestItr == mRequestList.end())
    {
        retVal = AS_NOT_FOUND;
    }

    return retVal;
}

/** Get all issued requests. */
std::list<IAsyncHttpRequestPtr> AsyncHttpSocketConnection::getAllIssuedRequests() const
{
    std::list<IAsyncHttpRequestPtr> retList;

    RequestList::const_iterator requestItr = mRequestList.begin();

    while (requestItr != mRequestList.end())
    {
        retList.push_back( *requestItr );

        requestItr++;
    }

    return retList;
}

/** Get the triggering request. */
IAsyncHttpRequestPtr AsyncHttpSocketConnection::getTriggeringRequest()
{
    AsyncHttpSocketRequestPtr retPtr;

    if ((!mRequestList.empty())
        && ((mRequestList.front()->getState() == IAsyncHttpRequest::COMPLETED_STATE)
            || (mRequestList.front()->getState() == IAsyncHttpRequest::RECEIVING_BODY_STATE)))
    {
        retPtr = mRequestList.front();

        if (retPtr->getResponseBodyLength() == 0)
        {
            mRequestList.pop_front();

            if (mRequestList.empty())
            {
                transit( IDLE_STATE );
            }
        }
    }

    return retPtr;
}


/** Pause the receiving on this connection. */
int32_t AsyncHttpSocketConnection::pauseReceiving()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    if (!mReceivingPausedFlag)
    {
        AseTimeVal timeNow = AseTimeVal::now();

        //If there is an outstanding response available event, clear it.
        //For state change events, we still want them processed.
        if (mOutstandingEvent == RESPONSE_AVAILABLE_EVENT)
        {
            mOutstandingEvent = NO_EVENT;
        }

        if (mState == ACTIVE_STATE)
        {
            //Indicate receiving is paused on all the requests.
            RequestList::iterator itrRequest;

            itrRequest = mRequestList.begin();

            while (itrRequest != mRequestList.end())
            {
                (*itrRequest)->pauseReceiving( timeNow );

                itrRequest++;
            }

            mFirstReceiveAfterResume = true;
        }

        mPausedTime = timeNow;

        mReceivingPausedFlag = true;
    }

    retVal = AS_NO_ERROR;

    return retVal;
}

/** Resume the receiving on this connection. */
int32_t AsyncHttpSocketConnection::resumeReceiving()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    if (mReceivingPausedFlag)
    {
        if (mState == ACTIVE_STATE)
        {
            AseTimeVal timeNow = AseTimeVal::now();

            //Indicate receiving is paused on all the requests.
            RequestList::iterator itrRequest;

            itrRequest = mRequestList.begin();

            while (itrRequest != mRequestList.end())
            {
                (*itrRequest)->resumeReceiving( timeNow );

                itrRequest++;
            }

            if ((timeNow - mPausedTime).ms() > 50)
            {
                mIgnoredDataLength = 0;
            }

            mTimerOne = timeNow + mHttpStallTimeout;
        }

        mReceivingPausedFlag = false;
    }

    retVal = AS_NO_ERROR;

    return retVal;

}

/** Transfer the socket handle to the caller. */
int32_t AsyncHttpSocketConnection::transferSocketHandle()
{
    assert( mState == IDLE_STATE );

    int32_t retSocketHandle;

    retSocketHandle = mSocketHandle;

    mSocketHandle = -1;

    return retSocketHandle;
}

/** Parse response status line. */
void AsyncHttpSocketConnection::parseResponseStatus( int32_t& responseStatus )
{
    assert( responseStatus == AS_NO_ERROR );

    bool statusReady = false;

    /** Status Line:    <HTTP-Version> <SP> <Status-Code> <SP> <Reason-Phrase> <CRLF>
     *  Item Index:  0  1              2    3             4    5                6 7
     */

    //Go through received bytes. The loop exits when one of the following is true:
    // (1) The response is found invalid.
    // (2) The full response status line is parsed.
    // (3) More data is needed for the status line.
    while (responseStatus == AS_NO_ERROR
           && !statusReady
           && (mProcessingPtr < (mReceivingBuffer + mReceivedDataLength)))
    {
        //Look at where it is in terms of receiving and parsing the response line.
        switch (mItemIndex)
        {
            case 0:
            case 2:
            case 4:
            {
                if (*mProcessingPtr != ' ')
                {
                    mItemPtr = mProcessingPtr;

                    mItemIndex++;
                }

                break;
            }

            case 1:
            {
                if (*mProcessingPtr == ' ')
                {
                    //The HTTP-version is available.
                    mStatusHttpVersion = std::string(
                                                (const char*) mItemPtr,
                                                (mProcessingPtr - mItemPtr) );

                    mItemPtr = NULL;

                    mItemIndex++;
                }

                break;
            }

            case 3:
            {
                if (*mProcessingPtr == ' ')
                {
                    //The Status-Code is available.
                    mStatusCode = std::string(
                                            (const char*) mItemPtr,
                                            (mProcessingPtr - mItemPtr) );

                    mItemPtr = NULL;

                    mItemIndex++;
                }

                break;
            }

           case 5:
            {
                if (*mProcessingPtr == '\r')
                {
                    //The Reason-Phrase is available.
                    mStatusReasonPhrase = std::string(
                                            (const char*) mItemPtr,
                                            mProcessingPtr - mItemPtr );

                    mItemIndex++;

                    mItemPtr = NULL;
                }

                break;
            }

            case 6:
            {
                if (*mProcessingPtr == '\n')
                {
                    (*mReceivingRequestItr)->setResponseStatus(
                                                        mStatusHttpVersion,
                                                        mStatusCode,
                                                        mStatusReasonPhrase );

                    (*mReceivingRequestItr)->setState(
                                                IAsyncHttpRequest::RECEIVING_HEADERS_STATE );

                    mItemIndex = 0;

                    mItemPtr = NULL;

                    statusReady = true;
                }
                else
                {
                    responseStatus = AS_HTTP_PROTOCOL_ERROR;
                }

                break;
            }
        }

        mProcessingPtr++;
    }
}

/** Parse response headers. */
void AsyncHttpSocketConnection::parseResponseHeaders( int32_t& responseStatus )
{
    assert( responseStatus == AS_NO_ERROR );

    bool headersReady = false;

    /** Headers:        <Header-Name> <":"> <SP/HT> <Header-Value> <CRLF>
     * Item Index:  0   1                   2       3               4 5
     *
     *                  <Last CRLF>
     *                        6 7
     */

    //Go through received bytes. The loop exits when one of the following is true:
    // (1) The response is found invalid.
    // (2) All the headers are parsed.
    // (3) More data is needed for headers.
    while (responseStatus == AS_NO_ERROR
           && !headersReady
           && (mProcessingPtr < (mReceivingBuffer + mReceivedDataLength)))
    {
        //Look at where it is in terms of receiving and parsing the headers.
        switch (mItemIndex)
        {
            case 0:
            {
                if ((*mProcessingPtr != ' ') && (*mProcessingPtr != '\t'))
                {
                    mItemPtr = mProcessingPtr;

                    mItemIndex++;
                }

                break;
            }

            case 1:
            {
                if (*mProcessingPtr == ':')
                {
                    //The header name is available.
                    (*mReceivingRequestItr)->addResponseHeaderName(
                                                            mItemPtr,
                                                            mProcessingPtr - mItemPtr );

                    mItemPtr = NULL;

                    mItemIndex++;
                }

                break;
            }

            case 2:
            {
                if ((*mProcessingPtr != ' ')
                    && (*mProcessingPtr != '\t')
                    && (*mProcessingPtr != '\r'))
                {
                    mItemPtr = mProcessingPtr;

                    mItemIndex++;

                }
                else if (*mProcessingPtr == '\r')
                {
                    mItemIndex = 4;
                }

                break;
            }

            case 3:
            {
                if (*mProcessingPtr == '\r')
                {
                    //Complete or partial header value is available.
                    (*mReceivingRequestItr)->addResponseHeaderValue(
                                                            mItemPtr,
                                                            mProcessingPtr - mItemPtr );

                    mItemPtr = NULL;

                    mItemIndex++;
                }

                break;
            }

            case 4:
            {
                if (*mProcessingPtr == '\n')
                {
                    mItemIndex++;
                }
                else
                {
                    responseStatus = AS_HTTP_PROTOCOL_ERROR;
                }

                break;
            }

            case 5:
            {
                if ((*mProcessingPtr == ' ')
                    || (*mProcessingPtr == '\t'))
                {
                    mItemIndex = 2;
                }
                else if (*mProcessingPtr == '\r')
                {
                    responseStatus = (*mReceivingRequestItr)->signalResponseHeaderCompleted();

                    mItemIndex++;
                }
                else
                {
                    responseStatus = (*mReceivingRequestItr)->signalResponseHeaderCompleted();

                    mItemPtr = mProcessingPtr;

                    mItemIndex = 1;
                }

                break;
            }

            case 6:
            {
                if (*mProcessingPtr == '\n')
                {
                    mItemPtr = NULL;

                    mItemIndex = 0;

                    headersReady = true;

                    //Check this response has a body.
                    if ((*mReceivingRequestItr)->getResponseBodyLength() == 0)
                    {
                        // Drain the data we peeked at to get here
                        if(mMinimumRecvMode)
                        {
                            uint32_t const bufferLength = (mProcessingPtr + 1) - mReceivingBuffer;

                            recv(mSocketHandle,
                                 mReceivingBuffer,
                                 bufferLength,
                                 0);

                            mAsyncHttpSocketClientPtr->trafficShaperConsume( bufferLength );
                        }

                        //If there is no body, flag this request as completed.
                        (*mReceivingRequestItr)->setState(
                                                    IAsyncHttpRequest::COMPLETED_STATE );

                        setupNextReceivingRequest();
                    }
                    else
                    {
                        (*mReceivingRequestItr)->setState(IAsyncHttpRequest::RECEIVING_BODY_STATE );

                        mHasNonEmptyChunks = false;
                    }
                }
                else
                {
                    responseStatus = AS_HTTP_PROTOCOL_ERROR;
                }

                break;
            }
        }

        mProcessingPtr++;
    }
}

/** Parse chunked response body - chunk size. */
void AsyncHttpSocketConnection::parseChunkSize( int32_t& responseStatus )
{
    assert( responseStatus == AS_NO_ERROR );

    bool sizeReady = false;

    /** Chunk Size:    <Chunk Size> <SP> <CRLF>
     *  Item Index:  0  1            2     3
     */

    //Go through received bytes. The loop exits when one of the following is true:
    // (1) The response is found invalid.
    // (2) The chunk size is parsed.
    // (3) More data is needed for the status line.
    while (responseStatus == AS_NO_ERROR
           && !sizeReady
           && (mProcessingPtr < (mReceivingBuffer + mReceivedDataLength)))
    {
        //Look at where it is in terms of receiving and parsing the chunk size.
        switch (mItemIndex)
        {
            case 0:
            {
                if (((*mProcessingPtr >= '0') && (*mProcessingPtr <= '9'))
                    || ((*mProcessingPtr >= 'A') && (*mProcessingPtr <= 'F'))
                    || ((*mProcessingPtr >= 'a') && (*mProcessingPtr <= 'f')))
                {
                    mCurrentChunkSize = 0;

                    mItemIndex++;
                }
                else if (*mProcessingPtr == ' ')
                {
                    break;
                }
                else
                {
                    responseStatus = AS_HTTP_PROTOCOL_ERROR;

                    break;
                }
            }

            case 1:
            {
                if ((*mProcessingPtr >= '0') && (*mProcessingPtr <= '9'))
                {
                    mCurrentChunkSize = mCurrentChunkSize * 16 + (*mProcessingPtr) - '0';
                }
                else if ((*mProcessingPtr >= 'A') && (*mProcessingPtr <= 'F'))
                {
                    mCurrentChunkSize = mCurrentChunkSize * 16 + 10 + (*mProcessingPtr) - 'A';
                }
                else if ((*mProcessingPtr >= 'a') && (*mProcessingPtr <= 'f'))
                {
                    mCurrentChunkSize = mCurrentChunkSize * 16 + 10 + (*mProcessingPtr) - 'a';
                }
                else if ((*mProcessingPtr == ' ') || (*mProcessingPtr == '\t'))
                {
                    mItemIndex++;
                }
                else if (*mProcessingPtr == '\r')
                {
                    mItemIndex = 3;
                }
                else
                {
                    responseStatus = AS_HTTP_PROTOCOL_ERROR;
                }

                break;
            }

            case 2:
            {
                if (*mProcessingPtr == '\r')
                {
                    mItemIndex++;
                }
                else if ((*mProcessingPtr != ' ') && (*mProcessingPtr != '\t'))
                {
                    responseStatus = AS_HTTP_PROTOCOL_ERROR;
                }

                break;
            }

            case 3:
            {
                if (*mProcessingPtr == '\n')
                {
                    sizeReady = true;

                    if (mCurrentChunkSize == 0)
                    {
                        (*mReceivingRequestItr)->setChunkedTransferState(
                                    AsyncHttpSocketRequest::RECEIVING_CHUNK_TRAILER_STATE);
                    }
                    else
                    {
                        (*mReceivingRequestItr)->setChunkedTransferState(
                                    AsyncHttpSocketRequest::RECEIVING_CHUNK_DATA_BODY_STATE);
                         if (mProcessingPtr + 1 == (mReceivingBuffer + mReceivedDataLength))
                         {
                             (*mReceivingRequestItr)->addStagedChunk(0, 0, mCurrentChunkSize);
                         }

                         mHasNonEmptyChunks = true;
                    }

                    mItemIndex = 0;

                    mItemPtr = NULL;
                }
                else
                {
                    responseStatus = AS_HTTP_PROTOCOL_ERROR;
                }

                break;
            }
        }

        mProcessingPtr++;
    }
}

/** Parse chunked response body - chunk data body. */
void AsyncHttpSocketConnection::parseChunkDataBody( int32_t& /*responseStatus*/ )
{
    //This is a chunk data body. Check if the entire body is in the staging buffer.
    if (mCurrentChunkSize<= (mReceivedDataLength - (mProcessingPtr - mReceivingBuffer)))
    {
        //The entire chunk is in the staging buffer. Add the entire chunk to the request.
        (*mReceivingRequestItr)->addStagedChunk(
                                            mProcessingPtr,
                                            mCurrentChunkSize,
                                            mCurrentChunkSize );

        mProcessingPtr += mCurrentChunkSize;

        (*mReceivingRequestItr)->setChunkedTransferState(
                                    AsyncHttpSocketRequest::RECEIVING_CHUNK_DATA_END_STATE );
    }
    else
    {
        //Only partial body is in the staging buffer, add the partial body to the request.
        (*mReceivingRequestItr)->addStagedChunk(
                                    mProcessingPtr,
                                    mReceivedDataLength - (mProcessingPtr - mReceivingBuffer),
                                    mCurrentChunkSize );

        mProcessingPtr = mReceivedDataLength + mReceivingBuffer;
    }
}

/** Parse chunked response body - chunk data end. */
void AsyncHttpSocketConnection::parseChunkDataEnd( int32_t& responseStatus )
{
    assert( responseStatus == AS_NO_ERROR );

    bool endReady = false;

    /** Chunk Data End:    <CRLF>
     *  Item Index:     0   1 2
     */

    //Go through received bytes. The loop exits when one of the following is true:
    // (1) The response is found invalid.
    // (2) The chunk body end is parsed.
    // (3) More data is needed for the status line.
    while (responseStatus == AS_NO_ERROR
           && !endReady
           && (mProcessingPtr < (mReceivingBuffer + mReceivedDataLength)))
    {
        //Look at where it is in terms of receiving and parsing the chunk size.
        switch (mItemIndex)
        {
            case 0:
            {
                if (*mProcessingPtr == '\r')
                {
                    mItemIndex++;

                    break;
                }
                else
                {
                    responseStatus = AS_HTTP_PROTOCOL_ERROR;

                    break;
                }
            }

            case 1:
            {
                if (*mProcessingPtr == '\n')
                {
                    endReady = true;

                    (*mReceivingRequestItr)->setChunkedTransferState(
                                    AsyncHttpSocketRequest::RECEIVING_CHUNK_SIZE_STATE );

                    mItemIndex = 0;

                    mItemPtr = NULL;
                }
                else
                {
                    responseStatus = AS_HTTP_PROTOCOL_ERROR;
                }

                break;
            }
        }

        mProcessingPtr++;
    }
}


/** Parse chunked response body - chunk trailer. */
void AsyncHttpSocketConnection::parseChunkTrailer( int32_t& responseStatus )
{
    assert(responseStatus == AS_NO_ERROR);
    bool trailerReady = false;

    /** TrailerHeaders: <Header-Name> <":"> <SP/HT> <Header-Value><CRLF>
     * Item Index:  0   1                   2       3              4 5
     *
     *                  <Last CRLF>
     *                        6 7
     */

    //Go through received bytes. The loop exits when one of the following is true:
    // (1) The response is found invalid.
    // (2) All the headers are parsed.
    // (3) More data is needed for headers.
    while (responseStatus == AS_NO_ERROR
           && !trailerReady
           && (mProcessingPtr < (mReceivingBuffer + mReceivedDataLength)))
    {
        //Look at where it is in terms of receiving and parsing the headers.
        switch (mItemIndex)
        {
            case 0:
            {
                if (*mProcessingPtr == '\r')
                {
                    mItemIndex = 6;
                }
                else if ((*mProcessingPtr != ' ') && (*mProcessingPtr != '\t'))
                {
                    mItemPtr = mProcessingPtr;

                    mItemIndex++;
                }

                break;
            }

            case 1:
            {
                if (*mProcessingPtr == ':')
                {
                    //The header name is available.
                    (*mReceivingRequestItr)->addResponseHeaderName(
                                                            mItemPtr,
                                                            mProcessingPtr - mItemPtr );

                    mItemPtr = NULL;

                    mItemIndex++;
                }

                break;
            }

            case 2:
            {
                if ((*mProcessingPtr != ' ')
                    && (*mProcessingPtr != '\t')
                    && (*mProcessingPtr != '\r'))
                {
                    mItemPtr = mProcessingPtr;

                    mItemIndex++;

                }
                else if (*mProcessingPtr == '\r')
                {
                    mItemIndex = 4;
                }

                break;
            }

            case 3:
            {
                if (*mProcessingPtr == '\r')
                {
                    //Complete or partial header value is available.
                    (*mReceivingRequestItr)->addResponseHeaderValue(
                                                            mItemPtr,
                                                            mProcessingPtr - mItemPtr );

                    mItemPtr = NULL;

                    mItemIndex++;
                }

                break;
            }

            case 4:
            {
                if (*mProcessingPtr == '\n')
                {
                    (*mReceivingRequestItr)->signalResponseHeaderCompleted();

                    mItemIndex++;
                }
                else
                {
                    responseStatus = AS_HTTP_PROTOCOL_ERROR;
                }

                break;
            }

            case 5:
            {
                if ((*mProcessingPtr == ' ')
                    || (*mProcessingPtr == '\t'))
                {
                    mItemIndex = 2;
                }
                else if (*mProcessingPtr == '\r')
                {
                    mItemIndex++;
                }
                else
                {
                    mItemPtr = mProcessingPtr;

                    mItemIndex = 1;
                }

                break;
            }

            case 6:
            {
                if (*mProcessingPtr == '\n')
                {
                    //Set this as an empty body
                    if ( !mHasNonEmptyChunks )
                    {
                        (*mReceivingRequestItr)->setResponseBodyEmpty();
                    }

                    //Flag this request as completed.
                    (*mReceivingRequestItr)->setState(
                                                IAsyncHttpRequest::COMPLETED_STATE );

                    setupNextReceivingRequest();

                    mItemPtr = NULL;

                    mItemIndex = 0;

                    trailerReady = true;
                }
                else
                {
                    responseStatus = AS_HTTP_PROTOCOL_ERROR;
                }

                break;
            }
        }

        mProcessingPtr++;
    }
}

/** Parse chunked response body. */
void AsyncHttpSocketConnection::parseChunkedResponseBody( int32_t& responseStatus )
{
    unsigned char * const origProcessingPtr = mProcessingPtr;

    RequestList::iterator currentReceivingRequestItr = mReceivingRequestItr;

    //Look at the current chunked transfer state of the receiving request.
    AsyncHttpSocketRequest::ChunkedTransferState state = (*mReceivingRequestItr)->getChunkedTransferState();

    switch ( state )
    {
        case AsyncHttpSocketRequest::RECEIVING_CHUNK_SIZE_STATE:
        {
            parseChunkSize( responseStatus );

            break;
        }

        case AsyncHttpSocketRequest::RECEIVING_CHUNK_DATA_BODY_STATE:
        {
            parseChunkDataBody( responseStatus );

            break;
        }

        case AsyncHttpSocketRequest::RECEIVING_CHUNK_DATA_END_STATE:
        {
            parseChunkDataEnd( responseStatus );

            break;
        }

        case AsyncHttpSocketRequest::RECEIVING_CHUNK_TRAILER_STATE:
        {
            parseChunkTrailer( responseStatus );

            break;
        }
    }

    if ( state != AsyncHttpSocketRequest::RECEIVING_CHUNK_DATA_BODY_STATE )
    {
        (*currentReceivingRequestItr)->indicateHeaderBytesReceived(
            AseTimeVal::now(),
            mProcessingPtr - origProcessingPtr,
            (*currentReceivingRequestItr)->getState() == IAsyncHttpRequest::COMPLETED_STATE );
    }
}

/** Parse response body. */
void AsyncHttpSocketConnection::parseResponseBody( int32_t& responseStatus )
{
    assert( responseStatus == AS_NO_ERROR );

    if ((*mReceivingRequestItr)->getChunkedTransferFlag())
    {
        parseChunkedResponseBody( responseStatus );
    }
    else
    {
        //This is a regular response body. Check if the entire response body is in
        //the staging buffer.
        if ((*mReceivingRequestItr)->getResponseBodyLength() >= 0
            && (*mReceivingRequestItr)->getResponseBodyLength() <= (mReceivedDataLength - (mProcessingPtr - mReceivingBuffer)))
        {
            //The entire body is in the staging buffer. Add the entire body to the request.
            (*mReceivingRequestItr)->addStagedBody(
                                        mProcessingPtr,
                                        (uint32_t) (*mReceivingRequestItr)->getResponseBodyLength() );

            mProcessingPtr += (*mReceivingRequestItr)->getResponseBodyLength();

            if(mMinimumRecvMode)
            {
                // Drain the data we peeked at to get here
                uint32_t const bufferLength = mProcessingPtr - mReceivingBuffer;

                recv(mSocketHandle,
                     mReceivingBuffer,
                     bufferLength,
                     0);

                mAsyncHttpSocketClientPtr->trafficShaperConsume( bufferLength );
            }

            //Flag this request as completed.
            (*mReceivingRequestItr)->setState(
                                        IAsyncHttpRequest::COMPLETED_STATE );

            setupNextReceivingRequest();
        }
        else
        {
            //Only partial body is in the staging buffer, add the partial body to the request.
            (*mReceivingRequestItr)->addStagedBody(
                                        mProcessingPtr,
                                        mReceivedDataLength - (mProcessingPtr - mReceivingBuffer));

            mProcessingPtr = mReceivedDataLength + mReceivingBuffer;

            if(mMinimumRecvMode)
            {
                // Drain the data we peeked at to get here
                uint32_t const bufferLength = mProcessingPtr - mReceivingBuffer;

                recv(mSocketHandle,
                     mReceivingBuffer,
                     bufferLength,
                     0);

               mAsyncHttpSocketClientPtr->trafficShaperConsume( bufferLength );
            }
        }
    }
}


/** Parse some responses. */
void AsyncHttpSocketConnection::parseResponses()
{
    AseTimeVal now = AseTimeVal::now();

    //The business of decoding HTTP resonses is about finding "\r\n". Let's be efficient
    //and work out more things while looking for "\r\n".

    int32_t responseStatus = AS_NO_ERROR;

    //Go through every recevied byte and work out what to do with it.
    while ((mReceivingRequestItr != mRequestList.end())
           && (mProcessingPtr < (mReceivingBuffer + mReceivedDataLength)))
    {
        unsigned char * const origProcessingPtr = mProcessingPtr;

        AsyncHttpSocketRequestPtr pRequest( *mReceivingRequestItr );

        //Look at the current state of the request of which the response is being received.
        switch (pRequest->getState())
        {
            //This is the first time something is received for the request.
            case IAsyncHttpRequest::WAITING_STATE:
            {
                //Move The receiving request the state to receive the status line.
                pRequest->setState( IAsyncHttpRequest::RECEIVING_STATUS_STATE );

                mItemIndex = 0;
            }

            case IAsyncHttpRequest::RECEIVING_STATUS_STATE:
            {
                parseResponseStatus( responseStatus );

                pRequest->indicateHeaderBytesReceived( now, mProcessingPtr - origProcessingPtr );

                break;
            }

            case IAsyncHttpRequest::RECEIVING_HEADERS_STATE:
            {
                parseResponseHeaders( responseStatus );

                pRequest->indicateHeaderBytesReceived( now, mProcessingPtr - origProcessingPtr );

                break;
            }

            case IAsyncHttpRequest::RECEIVING_BODY_STATE:
            {
                parseResponseBody( responseStatus );

                break;
            }

            default:
            {
                break;
            }
        }

        if (responseStatus != AS_NO_ERROR)
        {

            transit( FAILED_STATE );
            mConnectionFailureCode = responseStatus;

            mLowLevelFailureCode = 0;

            ::close( mSocketHandle );

            mSocketHandle = -1;

            if (mTraceListenerPtr)
            {
                mTraceListenerPtr->reportTcpDisconnect(
                    mId,
                    mHttpId,
                    mConnectionFailureCode,
                    mLowLevelFailureCode,
                    TCP_DISCONNECT_BY_REMOTE_WHILE_RECEIVING,
                    (mReceivingRequestItr != mRequestList.end())
                     && ((*mReceivingRequestItr)->getState() > IAsyncHttpRequest::WAITING_STATE) );
            }

            mOutstandingEvent = STATE_CHANGE_EVENT;

            break;
        }
    }
}

/** Receive responses. */
int32_t AsyncHttpSocketConnection::receiveResponses()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    IAsyncHttpRequest::State requestState;

    requestState = (*mReceivingRequestItr)->getState();

    //Put the received data into staging buffer under these conditions.
    if ((requestState == IAsyncHttpRequest::WAITING_STATE)
        || (requestState == IAsyncHttpRequest::RECEIVING_STATUS_STATE)
        || (requestState == IAsyncHttpRequest::RECEIVING_HEADERS_STATE)
        || ((requestState == IAsyncHttpRequest::RECEIVING_BODY_STATE)
            && ((*mReceivingRequestItr)->getChunkedTransferFlag())
            && ((*mReceivingRequestItr)->getChunkedTransferState()
                        != AsyncHttpSocketRequest::RECEIVING_CHUNK_DATA_BODY_STATE)))
    {
        //Reset the receiving staging buffer before calling receivev().
        resetReceivingBuffer();

        retVal = receive(
                    mReceivingBuffer + mReceivedDataLength,
                    RECEIVING_STAGING_BUFFER_SIZE - mReceivedDataLength,
                    0 );

        if (retVal > 0)
        {
            mReceivedDataLength += retVal;

            parseResponses();

            retVal = AS_NO_ERROR;
        }
        else if (retVal == 0)
        {

            transit( CLOSED_STATE );

            mConnectionFailureCode = AS_CONNECTION_CLOSED;
            mLowLevelFailureCode = 0;

            ::close( mSocketHandle );

            mSocketHandle = -1;

            if (mTraceListenerPtr)
            {
                mTraceListenerPtr->reportTcpDisconnect(
                    mId,
                    mHttpId,
                    mConnectionFailureCode,
                    mLowLevelFailureCode,
                    TCP_DISCONNECT_BY_REMOTE_WHILE_RECEIVING,
                    (mReceivingRequestItr != mRequestList.end())
                     && ((*mReceivingRequestItr)->getState() > IAsyncHttpRequest::WAITING_STATE) );
            }

            mOutstandingEvent = STATE_CHANGE_EVENT;

            retVal = AS_CONNECTION_CLOSED;
        }
        else
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                retVal = AS_IN_PROGRESS;
            }
            else
            {

                // in this case, connection should be closed


                transit( FAILED_STATE );

                mOutstandingEvent = STATE_CHANGE_EVENT;

                mLowLevelFailureCode = errno;

                if (errno == ECONNRESET)
                {
                    retVal = AS_CONNECTION_RESET_WHILE_RECEIVING;
                }
                else
                {
                    retVal = mapErrno( errno );
                }

                mConnectionFailureCode = retVal;

                ::close( mSocketHandle );

                mSocketHandle = -1;

                if (mTraceListenerPtr)
                {
                    mTraceListenerPtr->reportTcpDisconnect(
                        mId,
                        mHttpId,
                        mConnectionFailureCode,
                        mLowLevelFailureCode,
                        TCP_DISCONNECT_BY_REMOTE_WHILE_RECEIVING,
                        (mReceivingRequestItr != mRequestList.end())
                         && ((*mReceivingRequestItr)->getState() > IAsyncHttpRequest::WAITING_STATE) );
                }
            }
        }
    }
    else if (requestState == IAsyncHttpRequest::RECEIVING_BODY_STATE)
    {
        //Let the request receive the response body.
        mOutstandingEvent = RESPONSE_AVAILABLE_EVENT;

        retVal = AS_NO_ERROR;
    }

    return retVal;
}


/** Transit to a new state */
void AsyncHttpSocketConnection::transit( State state )
{
    if (mState != state)
    {
        mPreviousState = mState;

        mStateEnterTime = AseTimeVal::now();
        NTRACE(TRACE_HTTPLIB,
               "Transitioning %p %s:%d %d %llu -- %s => %s",
               this, mDestinationHostName.c_str(),
               mDestinationPortNumber,
               mSocketHandle,
               static_cast<ullong>((uintptr_t)mAuxUserDataPtr),
               stateNames[mState], stateNames[state]);
        mState = state;

        if (mState == FAILED_STATE)
        {
            mFailedTime = mStateEnterTime;
        }
    }

    return;
}

/** Set up the next reqeust to receive response. */
void AsyncHttpSocketConnection::setupNextReceivingRequest()
{
    mReceivingRequestItr++;

    if (mReceivingRequestItr != mRequestList.end())
    {
        if ((*mReceivingRequestItr)->getState() < IAsyncHttpRequest::WAITING_STATE)
        {
            mReceivingRequestItr = mRequestList.end();

            mTimerOne = AseTimeVal::INFINITE;
        }
        else
        {
            assert( (*mReceivingRequestItr)->getState() == IAsyncHttpRequest::WAITING_STATE );

            (*mReceivingRequestItr)->setTimeoutTime( AseTimeVal::now() );
        }
    }
}

}
