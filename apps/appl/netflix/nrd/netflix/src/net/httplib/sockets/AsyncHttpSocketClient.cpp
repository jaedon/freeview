/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>

#include <nrdbase/config.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdbase/Log.h>
#include "AsyncHttpSocketClient.h"
#include "ConnectionPoolImpl.h"
#include <nrdnet/IAsyncHttpLib.h>

using namespace std;

const char *stateNames[] = {
    "LOOKING_UP_STATE",   /**< The DNS lookup is in progress. */
    "OPENING_STATE",          /**< The connection is being opened. */
    "IDLE_STATE",             /**< The connection has been opened and there is no request in progress. */
    "ACTIVE_STATE",           /**< The connection has been opened and there is at least one request in
                                 progress. */
    "CLOSED_STATE",           /**< The connection has been closed. */
    "FAILED_STATE"            /**< The connection has failed. */
};

namespace netflix {

/** Constructor. */
AsyncHttpSocketClient::AsyncHttpSocketClient() : mTrafficShaperConfigChanged( false )
                                               , mMaxExternalFd( -1 )
                                               , mConnectionFailureCode( 0 )
                                               , mLowLevelFailureCode( 0 )
                                               , mGlobalDnsCachePtr( NULL )
                                               , mDnsTraceListenerPtr( NULL )
                                               , mConnectionIdPool( 0 )
                                               , mConnectionReuse(false)
                                               , mMaxContentLength( 0 )
{
    mSocketPair[0] = mSocketPair[1] = -1;
    mIpConnectivityManagerPtr = &mDefaultIpConnectivityManager;
}

/** Destructor. */
AsyncHttpSocketClient::~AsyncHttpSocketClient()
{
}

/** Initialize the client. */
int32_t AsyncHttpSocketClient::init()
{
    int32_t retVal = AS_NO_ERROR;

    //Clear the connection map.
    mConnectionList.clear();

    // create the DNS client
    mClient = DnsManager::clientAdd("AsyncHttpSocketClient");

    if (!mClient)
    {
        retVal = AS_DNS_ERROR;
    }
    else
    {
        //Create the pipe/socket pair for inter-thread communication.
        retVal = pipe( mSocketPair );

        if (retVal < 0)
        {
            DnsManager::clientRemove(mClient);
            mClient.reset();
            mSocketPair[0] = mSocketPair[1] = -1;

            retVal = AS_SOCKET_ERROR;
        }
    }

    resetTrafficShaper();

    return retVal;
}

/** Deinitialize the client. */
int32_t AsyncHttpSocketClient::deinit()
{
    int32_t retVal = AS_NO_ERROR;

    if (mSocketPair[0] >= 0) {
        ::close(mSocketPair[0]);
        mSocketPair[0] = -1;
    }
    if (mSocketPair[1] >= 0) {
        ::close(mSocketPair[1]);
        mSocketPair[1] = -1;
    }

    DnsManager::clientRemove(mClient);
    mClient.reset();

    ConnectionList::iterator connectionItr;

    connectionItr = mConnectionList.begin();

    while (connectionItr != mConnectionList.end())
    {
        (*connectionItr)->close();

        connectionItr++;
    }

    if ( mIpConnectivityManagerPtr )
    {
        mIpConnectivityManagerPtr->removeIpConnectivityListener( shared_from_this() );
    }

    return retVal;
}

void AsyncHttpSocketClient::setConnectionReuse(bool enable)
{
    mConnectionReuse = enable;
}

/** Set the extra HTTP request headers. */
void AsyncHttpSocketClient::setExtraRequestHeaders( const HttpHeaders& httpHeaders )
{
    mExtraRequestHeaders = httpHeaders;
}

/** Get the extra HTTP request headers. */
const HttpHeaders& AsyncHttpSocketClient::getExtraRequestHeaders() const
{
    return mExtraRequestHeaders;
}

/** Set the trace route plugin. */
void AsyncHttpSocketClient::setTraceRoutePlugin( AsyncHttpTraceRoutePluginPtr pTraceRoutePlugin )
{
    mPluginList.push_back( pTraceRoutePlugin );
    mTraceRoutePluginPtr = pTraceRoutePlugin;
}

/** Open an HTTP connection. */
int32_t AsyncHttpSocketClient::openConnection(
                                    std::string const& hostName,
                                    uint16_t portNumber,
                                    const AseTimeVal& httpConnectionTimeout,
                                    const AseTimeVal& httpStallTimeout,
                                    uint32_t socketReceiveBuffer,
                                    IHttpConnectionTraceListener* pTraceListener,
                                    bool minimumRecvMode,
                                    IAsyncHttpConnectionPtr& pAsyncHttpConnection )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    // if connection is supposed to be reused, look one from connectionPool
    if( mConnectionReuse ){
        // need to mutex here
        AsyncHttpSocketConnectionPtr connectionPtr;
        connectionPtr = ConnectionPool::getConnection(hostName, portNumber);
        if (connectionPtr.get()){
            NTRACE(TRACE_HTTPLIB, "Reused a connection %s:%d state %s", hostName.c_str(), portNumber, stateNames[connectionPtr->getState()]);
            connectionPtr->setOutstandingEvent(IAsyncHttpConnection::STATE_CHANGE_EVENT);
            connectionPtr->setAsyncHttpSocketClient(this);
            connectionPtr->setConnectionTraceListener(pTraceListener);
            mConnectionList.push_back( connectionPtr );
            pAsyncHttpConnection = connectionPtr;
            return AS_NO_ERROR;
        }
    }

    // if connection is not found from connection pool, create  AsyncHttpSocketConntection object
    AsyncHttpSocketConnection* pHttpConnection;
    NTRACE(TRACE_HTTPLIB, "Opening a connection %s:%d", hostName.c_str(), portNumber);

    pHttpConnection = new AsyncHttpSocketConnection(
                                    this,
                                    allocateConnectionId(),
                                    hostName,
                                    pTraceListener );

    if (pHttpConnection)
    {
        pHttpConnection->setMaxContentLength(mMaxContentLength);
        AsyncHttpSocketConnectionPtr connectionPtr( pHttpConnection );

        pHttpConnection->setConnectionReuse(mConnectionReuse);

        pHttpConnection->setDestinationPortNumber( htons( portNumber ) );

        if (socketReceiveBuffer > 0)
        {
            pHttpConnection->setSocketReceiveBufferSize( socketReceiveBuffer );
        }
        else
        {
            pHttpConnection->setSocketReceiveBufferSize(
                                    mIpConnectivityManagerPtr->getSocketReceiveBufferSize() );
        }

        pHttpConnection->setSecondaryTcpConnectionDelay( AseTimeVal::fromMS(
                                mIpConnectivityManagerPtr->getSecondaryTcpConnectionDelay() ) );

        pHttpConnection->setFastTcpTimeoutDelay( AseTimeVal::fromMS(
                                mIpConnectivityManagerPtr->getFastTcpTimeoutDelay() ) );

        pHttpConnection->setFastTcpTimeoutMaxCount(
                                mIpConnectivityManagerPtr->getFastTcpTimeoutMaxCount() );

        pHttpConnection->setSocketReattemptDelays(
            AseTimeVal::fromMS( mIpConnectivityManagerPtr->getInitialSocketReattemptDelay() ),
            AseTimeVal::fromMS( mIpConnectivityManagerPtr->getMaximumSocketReattemptDelay() ) );

        if (httpConnectionTimeout != AseTimeVal::ZERO)
        {
            pHttpConnection->setHttpConnectionTimeout( httpConnectionTimeout );
        }
        else
        {
            pHttpConnection->setHttpConnectionTimeout( AseTimeVal::fromMS(
                                    mIpConnectivityManagerPtr->getHttpConnectionTimeout() ) );
        }

        if (httpStallTimeout != AseTimeVal::ZERO)
        {
            pHttpConnection->setHttpStallTimeout( httpStallTimeout );
        }
        else
        {
            pHttpConnection->setHttpStallTimeout( AseTimeVal::fromMS(
                                    mIpConnectivityManagerPtr->getHttpStallTimeout() ) );
        }

        pHttpConnection->setMinimumRecvMode(minimumRecvMode);

#ifdef NRDP_HAS_IPV6
        retVal = openIpV6V4Connection(
                                hostName,
                                connectionPtr );
#else
        retVal = openIpV4OnlyConnection(
                                hostName,
                                connectionPtr );
#endif

        if ((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS))
        {
            // this is list of AsyncHttpSocketConnection object regardless of
            // connected or not. If IpAddr is known, connectionPtr will be
            // object with connected socket, if not, connectionPtr will be
            // object that is in-fligt for address lookup

            mConnectionList.push_back( connectionPtr );
        }
        else
        {
            mConnectionFailureCode = retVal;
            mLowLevelFailureCode = pHttpConnection->getLowLevelFailureCode();

            // null connectionPtr object
            connectionPtr.reset();
        }

        pAsyncHttpConnection = connectionPtr;
        //NTRACE(TRACE_HTTPLIB, "returning new AsyncHttpConnection 0x%x", pAsyncHttpConnection.get());
    }
    else
    {
        retVal = AS_NOT_ENOUGH_MEMORY;
    }

    return retVal;
}

/** Close an HTTP connection. */
int32_t AsyncHttpSocketClient::closeConnection(IAsyncHttpConnectionPtr const& pAsyncHttpConnection,
                                                bool bAllowReuse,
                                                int32_t reason )
{
    NTRACE(TRACE_HTTPLIB, "Closed a connection %s state %s",
           pAsyncHttpConnection->getServerName().c_str(),
           stateNames[pAsyncHttpConnection->getState()]);

    int32_t retVal = AS_NOT_FOUND;

    ConnectionList::iterator connectionItr;

    //Look for the connection in the connection list.

    connectionItr = mConnectionList.begin();

    while (connectionItr != mConnectionList.end())
    {
        if ((*connectionItr).get() == pAsyncHttpConnection.get())
        {
            // see if this connection is in the middle of requesting DNS
            // resolution. if it is, remove request from DnsManager
            DnsRequestList::iterator rit = mRequests.begin();
            while (rit != mRequests.end())
            {
                DnsManager::RequestPtr request = *rit;
                AsyncHttpSocketConnection * connection = (AsyncHttpSocketConnection *)DnsManager::requestGetUserData(request);
                if (connection != connectionItr->get())
                {
                    ++rit;
                }
                else
                {
                    DnsManager::requestRemove(request);
                    rit = mRequests.erase(rit);
                }
            }

            if (mConnectionReuse && bAllowReuse){
                // if reused, put it to connection pool
                (*connectionItr)->resetReceivingBuffer();
                (*connectionItr)->setAsyncHttpSocketClient(NULL);

                if( ((*connectionItr)->getState() != IAsyncHttpConnection::IDLE_STATE)
                    ||  (!ConnectionPool::addConnection(*connectionItr)) ){
                    retVal = (*connectionItr)->close( reason );
                }

            } else {
                // if not reused, close the connection
                retVal = (*connectionItr)->close( reason );
            }

            (*connectionItr)->setConnectionTraceListener(NULL);

            mConnectionList.erase( connectionItr );

            break;
        }

        connectionItr++;
    }

    return retVal;
}

/** Get the connection failure code */
int32_t AsyncHttpSocketClient::getConnectionFailureCode() const
{
    return mConnectionFailureCode;
}

/** Get the low level failure code */
int32_t AsyncHttpSocketClient::getLowLevelFailureCode() const
{
    return mLowLevelFailureCode;
}

/** Wake up the HTTP client from select() */
int32_t AsyncHttpSocketClient::wake()
{
    return sendSignalByteExternal();
}

/** Send a signal byte from the external to the async HTTP library. */
int32_t AsyncHttpSocketClient::sendSignalByteExternal()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    char signalByte = 1;

    retVal = write(
                mSocketPair[1],
                &signalByte,
                1 );

    if (retVal == 1)
    {
        retVal = AS_NO_ERROR;
    }
    else
    {
        retVal = AS_SOCKET_ERROR;
    }

    return retVal;
}

/** Receive the signal byte from the internal of the async HTTP library. */
int32_t AsyncHttpSocketClient::receiveSignalByteInternal()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    //Receive the signal byte.
    char signalByte;

    retVal = read(
                mSocketPair[0],
                &signalByte,
                1 );

    if (retVal == 1)
    {
        retVal = AS_NO_ERROR;
    }
    else
    {
        retVal = AS_SOCKET_ERROR;
    }

    return retVal;
}

/** Set the external FD sets. */
void AsyncHttpSocketClient::setExternalFdSets(
                                    int maxFd,
                                    fd_set* pReadFdSet,
                                    fd_set* pWriteFdSet )
{
    mMaxExternalFd = maxFd;

    mExternalReadFdSet = *pReadFdSet;
    mExternalWriteFdSet = *pWriteFdSet;
}

/** Wait for events on connections or wake up signal. */
int32_t AsyncHttpSocketClient::select(
    std::vector<IAsyncHttpConnectionPtr>& selectedConnections,
    const AseTimeVal& timeout )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    selectedConnections.clear();

    ConnectionList::iterator connectionItr;

    PluginList::iterator pluginItr;

    if ( mTrafficShaperConfigChanged )
    {
        resetTrafficShaper();

        mTrafficShaperConfigChanged = false;
    }

    //Go through all the connections to look for outstanding events.
    connectionItr = mConnectionList.begin();

    while (connectionItr != mConnectionList.end())
    {
        if ((*connectionItr)->hasOutstandingEvent())
        {
            selectedConnections.push_back( *connectionItr );
        }

        connectionItr++;
    }

    if (!selectedConnections.empty())
    {
        int count;
        do {
            //There are outstanding events on some connections. We also want to
            //check if wake() is called.
            FD_ZERO( &mReadFdSet );
            FD_SET( mSocketPair[0], &mReadFdSet );

            struct timeval osSelectTimeVal = { 0, 0 };

            count = ::select(mSocketPair[0] + 1, &mReadFdSet, NULL, NULL, &osSelectTimeVal );
        } while (count == -1 && errno == EINTR);

        if (count == 1)
        {
            // both connection event (since selectedConnection is not empty) and wake up call
            retVal = SELECT_WAKE_AND_CONNECT;
        }
        else
        {
            // only connection event (since selectedConnection is not empty)
            retVal = SELECT_CONNECT;
        }
    }
    else
    {
        //Get the current time.
        AseTimeVal timeNow = AseTimeVal::now();

        //Compute when this select() should return because of timeout.
        AseTimeVal thisSelectTimeoutTime = AseTimeVal::INFINITE;

        if (timeout != AseTimeVal::INFINITE)
        {
            thisSelectTimeoutTime = timeNow + timeout;
        }

        //Start the inner select() loop.
        while (true)
        {
            if ( mTrafficShaperPtr )
            {
                mTrafficShaperPtr->sleep();

                timeNow = AseTimeVal::now();
            }

            if (timeNow > thisSelectTimeoutTime)
            {
                retVal = AS_NO_ERROR;

                break;
            }

            AseTimeVal osSelectTimeout = AseTimeVal::INFINITE;

            if (thisSelectTimeoutTime != AseTimeVal::INFINITE)
            {
                osSelectTimeout = thisSelectTimeoutTime - timeNow;
            }

            int maxFd = -1;

            FD_ZERO( &mReadFdSet );
            FD_ZERO( &mWriteFdSet );

            if (mMaxExternalFd != -1)
            {
                maxFd = mMaxExternalFd;

                mReadFdSet = mExternalReadFdSet;
                mWriteFdSet = mExternalWriteFdSet;
            }

            //Find out if there is any DNS lookup in progress.
            bool dnsLookupPending = false;
            int dnsSignalFd = -1;
            if (DnsManager::clientHasPendingRequests(mClient))
            {
                dnsSignalFd = DnsManager::clientGetSignalFd(mClient);

                if (dnsSignalFd >= 0)
                {
                    dnsLookupPending = true;

                    FD_SET(dnsSignalFd, &mReadFdSet);

                    if (maxFd < dnsSignalFd)
                    {
                        maxFd = dnsSignalFd;
                    }
                }
            }

            //Go through all the connections and prepare for select().
            //NTRACE(TRACE_HTTPLIB,"preparing %d connections for select", mConnectionList.size() );

            connectionItr = mConnectionList.begin();

            while (connectionItr != mConnectionList.end())
            {
                int selectingFd;

                selectingFd = -1;
                AseTimeVal selectingTimeout = AseTimeVal::INFINITE;

                (*connectionItr)->prepareSelect(
                    timeNow,
                    selectingFd,
                    &mReadFdSet,
                    &mWriteFdSet,
                    selectingTimeout );

                if (maxFd < selectingFd)
                {
                    maxFd = selectingFd;
                }

                //Find out the smallest timeout value.
                if ((osSelectTimeout == AseTimeVal::INFINITE)
                    && (selectingTimeout != AseTimeVal::INFINITE))
                {
                    osSelectTimeout = selectingTimeout;
                }
                else if ((osSelectTimeout != AseTimeVal::INFINITE)
                         && (selectingTimeout != AseTimeVal::INFINITE)
                         && (osSelectTimeout > selectingTimeout))
                {
                    osSelectTimeout = selectingTimeout;
                }

                connectionItr++;
            }

            //Go through all the plugins and prepare for select().
            pluginItr = mPluginList.begin();

            while (pluginItr != mPluginList.end())
            {
                int selectingFd;

                selectingFd = -1;
                AseTimeVal selectingTimeout;

                (*pluginItr)->prepareSelect(
                    timeNow,
                    selectingFd,
                    &mReadFdSet,
                    &mWriteFdSet,
                    selectingTimeout );

                if (maxFd < selectingFd)
                {
                    maxFd = selectingFd;
                }

                //Find out the smallest timeout value.
                if ((osSelectTimeout == AseTimeVal::INFINITE)
                    && (selectingTimeout != AseTimeVal::INFINITE))
                {
                    osSelectTimeout = selectingTimeout;
                }
                else if ((osSelectTimeout != AseTimeVal::INFINITE)
                         && (selectingTimeout != AseTimeVal::INFINITE)
                         && (osSelectTimeout > selectingTimeout))
                {
                    osSelectTimeout = selectingTimeout;
                }

                pluginItr++;
            }

            //Take the DNS lookup timeout into account and work out the OS select timeout.
            struct timeval osSelectTimeVal;
            struct timeval* pOsSelectTimeVal;

            if (osSelectTimeout != AseTimeVal::INFINITE)
            {
                osSelectTimeVal.tv_sec = (time_t) (osSelectTimeout.seconds());
                osSelectTimeVal.tv_usec = (suseconds_t) ((osSelectTimeout.ms() % 1000ULL)
                                                         * 1000ULL);

                pOsSelectTimeVal = &osSelectTimeVal;
            }
            else
            {
                pOsSelectTimeVal = NULL;
            }

            //Add the socket handle for wake up signal.
            FD_SET( mSocketPair[0], &mReadFdSet );

            if (maxFd < mSocketPair[0])
            {
                maxFd = mSocketPair[0];
            }

            //Call OS select.
            retVal = ::select(
                maxFd + 1,
                &mReadFdSet,
                &mWriteFdSet,
                NULL,
                pOsSelectTimeVal );

            //NTRACE(TRACE_HTTPLIB,"select returned %d, maxFd %d for select, %llums", retVal, maxFd, osSelectTimeout.ms() );

            if (retVal < 0)
            {
                ::usleep( 10000 );

                FD_ZERO( &mReadFdSet );
                FD_ZERO( &mWriteFdSet );

                NTRACE(TRACE_HTTPLIB,"select_error %d", errno );
            }

            //
            //Process pending DNS lookup if any.
            //
            if (dnsLookupPending && FD_ISSET(dnsSignalFd, &mReadFdSet))
            {
                //NTRACE(TRACE_HTTPLIB,"select dns, %d requests", mRequests.size() );

                DnsManager::clientClearSignal(mClient);

                DnsRequestList::iterator rit = mRequests.begin();
                while (rit != mRequests.end())
                {
                    DnsManager::RequestPtr request = *rit;
                    DnsManager::RequestResultPtr result = DnsManager::requestGetResult(request);
                    if (!result)
                    {
                        ++rit;
                    }
                    else
                    {
                        const DnsManager::RequestParams & params = DnsManager::requestGetParams(request);
                        AsyncHttpSocketConnection * connection = (AsyncHttpSocketConnection *)DnsManager::requestGetUserData(request);

                        if (DnsManager::resultIsUsable(result))
                        {
                            //DNS lookup succeeded
                            if (mDnsTraceListenerPtr)
                            {
                                std::vector<std::string> cnames;
                                cnames = result->mAliases;
                                cnames.push_back(result->mName);

                                std::vector<IpAddressTtlRecord> ipAddressTtlRecords;
                                for (DnsManager::RequestResult::Addresses::iterator ait = result->mAddresses.begin(); ait != result->mAddresses.end(); ++ait)
                                {
                                    IpAddressTtlRecord ipAddressTtlRecord;
                                    ipAddressTtlRecord.mIpAddress = (*ait).getStrAddr();
                                    ipAddressTtlRecord.mTtl = result->mTtl.seconds();
                                    ipAddressTtlRecords.push_back(ipAddressTtlRecord);
                                }

                                AseTimeVal lookupTime = AseTimeVal::fromMS(DnsManager::requestGetTime(request).ms());

                                DnsManager::Configuration dnsConfig;
                                DnsManager::getConfiguration(dnsConfig);

                                if (params.mAddressType == DnsManager::ADDRESSTYPE_IPV4)
                                {
                                    mDnsTraceListenerPtr->reportHostNameResolved(
                                        DNS_QUERY_SINGLE_A_TYPE,
                                        dnsConfig.mServers,
                                        timeNow - lookupTime,
                                        cnames,
                                        ipAddressTtlRecords );
                                }
#ifdef NRDP_HAS_IPV6
                                else if (params.mAddressType == DnsManager::ADDRESSTYPE_IPV6)
                                {
                                    mDnsTraceListenerPtr->reportHostNameResolved(
                                        DNS_QUERY_QUAD_A_TYPE,
                                        dnsConfig.mServers,
                                        timeNow - lookupTime,
                                        cnames,
                                        ipAddressTtlRecords );
                                }
#endif
                                else
                                {
                                    retVal = AS_DNS_QUERY_MALFORMED;
                                }
                            }

                            //Go ahead and open the connection.
                            if (connection)
                            {
                                connection->notifyDnsLookupResult(
                                    timeNow,
                                    AS_NO_ERROR,
                                    result->mAddresses,
                                    result->mName);
                            }
                        }
                        else
                        {
                            //DNS failed, set connection failure code correspondingly.

                            if (mDnsTraceListenerPtr)
                            {
                                AseTimeVal lookupTime = AseTimeVal::fromMS(DnsManager::requestGetTime(request).ms());

                                DnsManager::Configuration dnsConfig;
                                DnsManager::getConfiguration(dnsConfig);

                                if (params.mAddressType == DnsManager::ADDRESSTYPE_IPV4)
                                {
                                    mDnsTraceListenerPtr->reportDnsLookupFailure(
                                        DNS_QUERY_SINGLE_A_TYPE,
                                        dnsConfig.mServers,
                                        timeNow - lookupTime,
                                        params.mHostName,
                                        result->mErrorCode );
                                }
#ifdef NRDP_HAS_IPV6
                                else if (params.mAddressType == DnsManager::ADDRESSTYPE_IPV6)
                                {
                                    mDnsTraceListenerPtr->reportDnsLookupFailure(
                                        DNS_QUERY_QUAD_A_TYPE,
                                        dnsConfig.mServers,
                                        timeNow - lookupTime,
                                        params.mHostName,
                                        result->mErrorCode );
                                }
#endif
                                else
                                {
                                    retVal = AS_DNS_QUERY_MALFORMED;
                                }
                            }

                            if (connection)
                            {
                                connection->notifyDnsLookupResult(
                                    timeNow,
                                    result->mErrorCode,
                                    NrdIpAddr(),
                                    std::string());
                            }
                        }

                        DnsManager::requestRemove(request);

                        rit = mRequests.erase(rit);
                    }
                }
            }


            //Reget the current time.
            timeNow = AseTimeVal::now();

            //Go through all the plugins and process for select() events or timeouts.
            pluginItr = mPluginList.begin();

            while (pluginItr != mPluginList.end())
            {
                (*pluginItr)->processSelect(
                    timeNow,
                    &mReadFdSet,
                    &mWriteFdSet );

                pluginItr++;
            }

            /*
            if(mConnectionReuse){
                static int size = -1;
                if( size != mConnectionList.size()){
                    NTRACE(TRACE_HTTPLIB, " size of current connection list %d", mConnectionList.size());
                    size = mConnectionList.size();
                }
            }
            */

            //Go through all the connections and process select() events or timeouts.
            connectionItr = mConnectionList.begin();

            while (connectionItr != mConnectionList.end())
            {
                (*connectionItr)->processSelect(
                    timeNow,
                    &mReadFdSet,
                    &mWriteFdSet );

                if ((*connectionItr)->hasOutstandingEvent())
                {
                    selectedConnections.push_back( (*connectionItr) );
                }

                connectionItr++;
            }

            if (!selectedConnections.empty() && (FD_ISSET(mSocketPair[0], &mReadFdSet)))
            {
                retVal = SELECT_WAKE_AND_CONNECT;

                break;
            }
            else if (!selectedConnections.empty())
            {
                // return SELECT_CONNECT when there is event in connection
                retVal = SELECT_CONNECT;

                break;
            }
            else if (FD_ISSET( mSocketPair[0], &mReadFdSet ))
            {
                retVal = SELECT_WAKE;

                break;
            }
            else
            {
                if ((mMaxExternalFd != -1) && (retVal > 0))
                {
                    retVal = AS_NO_ERROR;

                    break;
                }

                //Figure out if we need to return because of timeout.
                if (timeNow >= thisSelectTimeoutTime)
                {
                    retVal = AS_NO_ERROR;

                    break;
                }
            }

            //Reset the current time.
            timeNow = AseTimeVal::now();

        }
    }

    return retVal;
}

/** Set the IP connectivity manager. */
void AsyncHttpSocketClient::setIpConnectivityManager(
                                        IpConnectivityManager* pIpConnectivityManager )
{
    if ( mIpConnectivityManagerPtr != pIpConnectivityManager )
    {
        if ( mIpConnectivityManagerPtr )
        {
            mIpConnectivityManagerPtr->removeIpConnectivityListener( shared_from_this() );
        }

        mIpConnectivityManagerPtr = pIpConnectivityManager;

        if ( mIpConnectivityManagerPtr )
        {
            mIpConnectivityManagerPtr->addIpConnectivityListener( shared_from_this() );

            resetTrafficShaper();
        }
    }
}

/** Get the IP connectivity manager. */
IpConnectivityManager* AsyncHttpSocketClient::getIpConnectivityManager()
{
    return mIpConnectivityManagerPtr;
}

/** Set the global DNS cache. */
void AsyncHttpSocketClient::setGlobalDnsCache( IDnsCache* pGlobalDnsCache )
{
    mGlobalDnsCachePtr = pGlobalDnsCache;
}

/** Set the DNS trace listener. */
void AsyncHttpSocketClient::setDnsTraceListener( IDnsTraceListener* pDnsTraceListener )
{
    mDnsTraceListenerPtr = pDnsTraceListener;
}

void AsyncHttpSocketClient::trafficShaperConfigChanged()
{
    mTrafficShaperConfigChanged = true;
}

/** Peek an ID for a new connection. */
uint32_t AsyncHttpSocketClient::peekConnectionId()
{
    return mConnectionIdPool;
}

/** Allocate an ID for a new connection. */
uint32_t AsyncHttpSocketClient::allocateConnectionId()
{
    return mConnectionIdPool++;
}

/** Trace route an IP address. */
void AsyncHttpSocketClient::traceRouteIpAddress( const NrdIpAddr& ipAddress )
{
    if (mTraceRoutePluginPtr)
    {
        mTraceRoutePluginPtr->traceRoute( ipAddress );
    }
}

#ifdef NRDP_HAS_IPV6
/** Open IpV6/V4 connection. */
int32_t AsyncHttpSocketClient::openIpV6V4Connection(
                                    const std::string& hostName,
                                    AsyncHttpSocketConnectionPtr pHttpConnection )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    IpConnectivityManager::Policy ipConnectivityPolicy;

    ipConnectivityPolicy = mIpConnectivityManagerPtr->getIpConnectivityPolicy();

    //Get the current time.
    AseTimeVal timeNow = AseTimeVal::now();

    do
    {
        NrdIpAddr nrdIpAddr( hostName );

        //Check if the host name is an IP literal.
        if (nrdIpAddr.valid())
        {
            if (nrdIpAddr.getVersion() == NrdIpAddr::IP_V4)
            {
                //It is an IPv4 literal.
                if (ipConnectivityPolicy != IpConnectivityManager::IP_V6_ONLY)
                {
                    pHttpConnection->setIpConnectivityPolicy(
                                            IpConnectivityManager::IP_V4_ONLY );

                    retVal = pHttpConnection->notifyDnsLookupResult(
                                                            timeNow,
                                                            AS_NO_ERROR,
                                                            nrdIpAddr,
                                                            std::string());
                }
                else
                {
                    //This is to purposely let pHttpConnection->notifyDnsLookupResult()
                    //handle the error.
                    pHttpConnection->setIpConnectivityPolicy(
                                            IpConnectivityManager::IP_V6_ONLY );

                    retVal = pHttpConnection->notifyDnsLookupResult(
                                                            timeNow,
                                                            AS_CONNECTION_INVALID_DESTINATION,
                                                            nrdIpAddr,
                                                            std::string());
                }
            }
            else
            {
                //It is an IPv6 literal.
                if (ipConnectivityPolicy != IpConnectivityManager::IP_V4_ONLY)
                {
                    pHttpConnection->setIpConnectivityPolicy(
                                            IpConnectivityManager::IP_V6_ONLY );

                    retVal = pHttpConnection->notifyDnsLookupResult(
                                                            timeNow,
                                                            AS_NO_ERROR,
                                                            nrdIpAddr,
                                                            std::string());
                }
                else
                {
                    //This is to purposely let pHttpConnection->notifyDnsLookupResult()
                    //handle the error.
                    pHttpConnection->setIpConnectivityPolicy(
                                            IpConnectivityManager::IP_V4_ONLY );

                    retVal = pHttpConnection->notifyDnsLookupResult(
                                                            timeNow,
                                                            AS_CONNECTION_INVALID_DESTINATION,
                                                            nrdIpAddr,
                                                            std::string());
                }
            }

            break;
        }

        //The host name is not an IP literal.
        pHttpConnection->setIpConnectivityPolicy( ipConnectivityPolicy );

        bool ipV4ResolutionNeeded = false;
        bool ipV6ResolutionNeeded = false;

        if ((ipConnectivityPolicy == IpConnectivityManager::IP_V4_ONLY)
            || (ipConnectivityPolicy == IpConnectivityManager::IP_V4_V6)
            || (ipConnectivityPolicy == IpConnectivityManager::IP_V6_V4))
        {
            ipV4ResolutionNeeded = true;
        }

        if ((ipConnectivityPolicy == IpConnectivityManager::IP_V6_ONLY)
            || (ipConnectivityPolicy == IpConnectivityManager::IP_V4_V6)
            || (ipConnectivityPolicy == IpConnectivityManager::IP_V6_V4))
        {
            ipV6ResolutionNeeded = true;
        }

        DnsManager::RequestParams params;
        params.mHostName = hostName;

        //Check if the host name can be looked up locally.
        if (ipV4ResolutionNeeded)
        {
            params.mAddressType = DnsManager::ADDRESSTYPE_IPV4;
            DnsManager::RequestResultPtr result = DnsManager::requestTryGetResult(params);

            if (DnsManager::resultIsUsable(result))
            {
                retVal = pHttpConnection->notifyDnsLookupResult(
                                                        timeNow,
                                                        AS_NO_ERROR,
                                                        result->mAddresses,
                                                        result->mName);

                ipV4ResolutionNeeded = false;
            }
        }

        if (ipV6ResolutionNeeded)
        {
            params.mAddressType = DnsManager::ADDRESSTYPE_IPV6;
            DnsManager::RequestResultPtr result = DnsManager::requestTryGetResult(params);

            if (DnsManager::resultIsUsable(result))
            {
                retVal = pHttpConnection->notifyDnsLookupResult(
                                                        timeNow,
                                                        AS_NO_ERROR,
                                                        result->mAddresses,
                                                        result->mName);

                ipV6ResolutionNeeded = false;
            }
        }

        if (!ipV4ResolutionNeeded && !ipV6ResolutionNeeded)
        {
            break;
        }

        if (ipV4ResolutionNeeded)
        {
            //The the resolved IP address is not available or is expired.
            //Issue a new DNS lookup request.

            params.mAddressType = DnsManager::ADDRESSTYPE_IPV4;
            if(DnsManager::RequestPtr request = DnsManager::requestAdd(mClient, params)){
                DnsManager::requestSetUserData(request, pHttpConnection.get());
                mRequests.push_back(request);
                retVal = AS_IN_PROGRESS;
            } else {
                // when name resolution fails more than configured retry limit,
                // dns manger will restart. No Request will be created while
                // restarting
                retVal = AS_DNS_NOT_INITIALIZED;
            }
        }

        if (ipV6ResolutionNeeded)
        {
            //The the resolved IP address is not available or is expired.
            //Issue a new DNS lookup request.

            params.mAddressType = DnsManager::ADDRESSTYPE_IPV6;
            if(DnsManager::RequestPtr request = DnsManager::requestAdd(mClient, params)){
                DnsManager::requestSetUserData(request, pHttpConnection.get());
                mRequests.push_back(request);
                retVal = AS_IN_PROGRESS;
            } else {
                // when name resolution fails more than configured retry limit,
                // dns manger will restart. No Request will be created while
                // restarting
                retVal = AS_DNS_NOT_INITIALIZED;
            }
        }

    } while (false);

    return retVal;
}

#else

/** Open IpV4 only connection. */
int32_t AsyncHttpSocketClient::openIpV4OnlyConnection(
                                    const std::string& hostName,
                                    AsyncHttpSocketConnectionPtr pHttpConnection )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    IpConnectivityManager::Policy ipConnectivityPolicy;

    ipConnectivityPolicy = mIpConnectivityManagerPtr->getIpConnectivityPolicy();

    assert( ipConnectivityPolicy == IpConnectivityManager::IP_V4_ONLY );

    //Get the current time.
    AseTimeVal timeNow = AseTimeVal::now();

    do
    {
        NrdIpAddr nrdIpAddr( hostName );

        pHttpConnection->setIpConnectivityPolicy( ipConnectivityPolicy );

        //Check if the host name is an IP literal.
        if (nrdIpAddr.valid())
        {
            retVal = pHttpConnection->notifyDnsLookupResult(
                                                    timeNow,
                                                    AS_NO_ERROR,
                                                    nrdIpAddr,
                                                    std::string() );

            break;
        }

        //The host name is not an IP literal.

        //Check if the host name can be looked up locally.

        DnsManager::RequestParams params;
        params.mHostName = hostName;
        params.mAddressType = DnsManager::ADDRESSTYPE_IPV4;

        DnsManager::RequestResultPtr result = DnsManager::requestTryGetResult(params);

        if (DnsManager::resultIsUsable(result))
        {
            retVal = pHttpConnection->notifyDnsLookupResult(
                                                    timeNow,
                                                    AS_NO_ERROR,
                                                    result->mAddresses,
                                                    result->mName);

            break;
        }

        //The the resolved IP address is not available or is expired.
        //Issue a new DNS lookup request.

        NTRACE(TRACE_HTTPLIB, "Making a DNS request %s:%d %p %d", hostName.c_str(),
               pHttpConnection->getDestinationPortNumber(),
               pHttpConnection.get(), pHttpConnection->getId());
        if (DnsManager::RequestPtr request = DnsManager::requestAdd(mClient, params)) {
            DnsManager::requestSetUserData(request, pHttpConnection.get());
            mRequests.push_back(request);

            retVal = AS_IN_PROGRESS;
        } else {
            retVal = AS_DNS_NOT_INITIALIZED;
        }

    } while (false );

    return retVal;
}

#endif

/** Reset the traffic shaper based on new configuration */
void AsyncHttpSocketClient::resetTrafficShaper()
{
    // Create the traffic shaper
    uint32_t const trafficShaperSpeed = mIpConnectivityManagerPtr->getTrafficShaperSpeed();
    if ( trafficShaperSpeed != 0 )
    {
        std::pair<uint32_t,uint32_t> const sleepIntervals = mIpConnectivityManagerPtr->getTrafficShaperSleepIntervals();
        std::pair<uint32_t,uint32_t> const capacities = mIpConnectivityManagerPtr->getTrafficShaperCapacities();

        NTRACE(TRACE_HTTPLIB, "TrafficShaper enabled %ukbit/s, sleep min/max: %u/%ums, capacity min/max %u/%u bytes",
            trafficShaperSpeed,
            sleepIntervals.first,
            sleepIntervals.second,
            capacities.first,
            capacities.second );

        mTrafficShaperPtr.reset( new TrafficShaper( trafficShaperSpeed,
                                                    sleepIntervals.first,
                                                    sleepIntervals.second,
                                                    capacities.first,
                                                    capacities.second ) );
    }
    else
    {
        NTRACE(TRACE_HTTPLIB, "TrafficShaper disabled" );
        mTrafficShaperPtr.reset();
    }
}

/** Construct an asynchronous HTTP client. */
int32_t constructAsyncHttpClient(
                    IAsyncHttpClientPtr& pAsyncHttpClient,
                    IpConnectivityManager* pIpConnectivityManager,
                    IDnsCache* pGlobalDnsCache,
                    IDnsTraceListener* pDnsTraceListener,
                    IRouteListener* pRouteListener,
                    bool enableConnectionReuse,
                    llong maxContentLength
    )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    //Initialize the socket based async HTTP client.
    AsyncHttpSocketClientPtr pAsyncHttpSocketClient;

    pAsyncHttpSocketClient = AsyncHttpSocketClientPtr( new AsyncHttpSocketClient );

    if (pAsyncHttpSocketClient)
    {
        pAsyncHttpSocketClient->setMaxContentLength(maxContentLength);
        pAsyncHttpSocketClient->setConnectionReuse(enableConnectionReuse);

        if (pIpConnectivityManager)
        {
            pAsyncHttpSocketClient->setIpConnectivityManager( pIpConnectivityManager );
        }

        if (pGlobalDnsCache)
        {
            pAsyncHttpSocketClient->setGlobalDnsCache( pGlobalDnsCache );
        }

        if (pDnsTraceListener)
        {
            pAsyncHttpSocketClient->setDnsTraceListener( pDnsTraceListener );
        }

// ifdef for NRDP_PLATFORM can't use negative logic because the stripping script can't parse for SDK build
        if (pRouteListener)
        {
            AsyncHttpTraceRoutePluginPtr pTraceRoutePlugin;

            pTraceRoutePlugin = AsyncHttpTraceRoutePluginPtr( new AsyncHttpTraceRoutePlugin );

            if (pTraceRoutePlugin)
            {
                retVal = pTraceRoutePlugin->init();

                if (retVal == AS_NO_ERROR)
                {
                    pTraceRoutePlugin->setRouteListener( pRouteListener );

                    pAsyncHttpSocketClient->setTraceRoutePlugin( pTraceRoutePlugin );
                }
            }
        }

        pAsyncHttpClient = pAsyncHttpSocketClient;

        retVal = AS_NO_ERROR;
    }
    else
    {
        retVal = AS_NOT_ENOUGH_MEMORY;
    }

    return retVal;
}

}
