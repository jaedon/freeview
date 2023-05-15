/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include <nrdbase/tr1.h>
#include <nrdnet/IAsyncHttpLib.h>

#include "HttpServiceRequest.h"

namespace netflix
{



class HttpServiceListener
{
public:
    virtual ~HttpServiceListener() {}
    // Called when a request is finished
    virtual void onHttpRequestFinished(int requestId) = 0;
};



class HttpService
{
public:

    HttpService();

    bool start(HttpServiceListener * listener, IAsyncHttpClientPtr client);
    void cleanUpRequests();
    void stop();

    void dump() const;

    // Perform HTTP operations (blocks)
    int service(std::vector<IAsyncHttpConnectionPtr> & selectedConnections, AseTimeVal const & timeout);

    // Add a HTTP request
    uint32_t addRequest(HttpServiceRequestPtr pServiceRequest);

    //Cancel a HTTP request.
    void cancelRequest(uint32_t requestId, const AseUrl & requestUrl);

    //Process the HTTP server connectioins.
    void processServerConnections(std::map<void *, std::vector<IAsyncHttpConnectionPtr> > const & servers,
                                  AseTimeVal const & timeNow);

    // Process the HTTP requests
    void processNewRequests(const AseTimeVal & timeNow);

    //The HTTP server record.
    class ServerRecord;

    //Type definition for Server Record pointer.
    typedef shared_ptr<ServerRecord> ServerRecordPtr;

    //Process timeout.
    std::vector<ServerRecordPtr> processTimeout(const AseTimeVal & timeNow);

    //Finish all the requests for a server.
    void finishServer(const AseTimeVal & timeNow, ServerRecord * pServerRecord);

    AseTimeVal findEarliestConnectTime(ullong max, AseTimeVal now);
    AseTimeVal earliestConnectTime() const { return mEarliestConnectTime; }
    bool removePendingRequest(uint32_t id);
    bool isPending(uint32_t id) const;
    bool isActive(uint32_t id, AseUrl const & url) const;

    //The HTTP pipelining depth.
    uint32_t mPipeliningDepth;

    //The HTTP pipelining support threshold.
    uint32_t mPipeliningSupportThreshold;

    //The HTTP connect backoff time if there is a connect failure, in ms.
    uint32_t mConnectBackoffTime;

    //The HTTP connect failure threshold, if there is this number of consecutive
    //connect failures, the server is deemed failed.
    uint32_t mConnectFailureThreshold;

    //The number of connections per server.
    uint32_t mNumberOfConnectionPerServer;

private:

    class RequestRecord;

    //Type definition for RequestRecord pointer.
    typedef shared_ptr<RequestRecord> RequestRecordPtr;

    //The HTTP request list.
    typedef std::list<RequestRecordPtr> RequestList;

    //The HTTP request map.
    typedef std::map<uint32_t, RequestRecordPtr> RequestMap;

    //The HTTP connection record.
    class ConnectionRecord;

    //Type definition for ConnectionRecord pointer.
    typedef shared_ptr<ConnectionRecord> ConnectionRecordPtr;

    //The HTTP connection vector.
    typedef std::vector<ConnectionRecordPtr> ConnectionVector;

    struct ServerKey
    {
        ServerKey(const AseUrl & url = AseUrl()) : port(url.GetPortNumber()), host(url.GetHost()) {}
        ServerKey(const uint16_t p, const std::string & h) : port(p), host(h) {}

        bool operator==(const ServerKey & other) const { return port == other.port && host == other.host; }
        bool operator<(const ServerKey & other) const { return port < other.port || (port == other.port && host < other.host); }
        std::string toString() const { std::ostringstream out; out << host << ':' << port; return out.str(); }

        const uint16_t port;
        const std::string host;
    };

    typedef std::map<ServerKey, ServerRecordPtr> ServerMap;

    void processServerConnections(const AseTimeVal & timeNow,
                                  ServerRecord * pServerRecord,
                                  std::vector<IAsyncHttpConnectionPtr> const & serverConnections);

    //Process the HTTP response.
    void processResponse(const AseTimeVal & timeNow,
                             ServerRecord * pServerRecord,
                             ConnectionRecord * pConnectionRecord);

    //Process the HTTP connection.
    void processConnection(const AseTimeVal & timeNow,
                               ServerRecord * pServerRecord,
                               ConnectionRecord * pConnectionRecord);

    //Process the HTTP failure.
    void processFailure(const AseTimeVal & timeNow,
                            ServerRecord * pServerRecord,
                            ConnectionRecord * pConnectionRecord);

    //Schedule HTTP requests for a serer.
    void scheduleHttpRequests(const AseTimeVal & timeNow, ServerRecord * pServerRecord);

    void finishHttpRequest(const AseTimeVal & timeNow,
                           RequestRecord * pRequestRecord,
                           int32_t aseErrorCode,
                           long httpStatusCode,
                           bool traceFlag = true);

    // Allocates a request ID
    uint32_t allocateId();

    //The HTTP server map.
    ServerMap mServerMap;

    //The new HTTP requests map.
    RequestMap mNewRequestMap;

    //The earliest connect time.
    AseTimeVal mEarliestConnectTime;

    //The async HTTP library.
    IAsyncHttpClientPtr mAsyncHttpClientPtr;

    // Notified when a request is finished
    HttpServiceListener * mListener;

    mutable Mutex mMutex;	// Protects mNewRequestMap

    // Source of request IDs
    static uint32_t mIdPool;
};



}


#endif // HTTP_SERVICE_H
