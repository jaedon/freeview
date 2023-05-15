/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef HTTPS_SERVICE_H
#define HTTPS_SERVICE_H


#include <nrdbase/tr1.h>
#include <nrdnet/IAsyncHttpLib.h>
#include <nrdnet/TrustStore.h>
#include "HttpServiceRequest.h"


namespace netflix
{

//Type definition for verifying certificate status.
typedef int (*VerifyCertStatusFunc)(
    int ok,
    void * ctx,
    HttpServiceRequest * request,
    const NrdIpAddr *ipAddress);


class HttpsServiceListener
{
public:
    virtual ~HttpsServiceListener() {}
    // Called when a request is finished
    virtual void onHttpsRequestFinished(int requestId) = 0;
};



class HttpsService
{
public:

    //The HTTPs request node.
    class Node;

    //Type definition for Node pointer.
    typedef shared_ptr<Node> NodePtr;

    // Flag in a request ID that signifies an HTTPS request
    static uint32_t const HTTPS_REQUEST_ID_MASK = 0x80000000;

    HttpsService();
    virtual ~HttpsService();

    void configure() const;
    bool start(HttpsServiceListener * listener, IAsyncHttpClientPtr client);
    void cleanUpRequests();
    void stop();

    //Performs HTTPs operations.
    void service(std::vector<IAsyncHttpConnectionPtr> & selectedConnections, const AseTimeVal & timeNow);

    // Adds a new request to be serviced
    uint32_t addRequest(HttpServiceRequestPtr pServiceRequest,
                        TrustStorePtr pTrustStore,
                        VerifyCertStatusFunc pVerifyCertStatusFunc,
                        bool sslVerifyPeerFlag);

    // Cancels an existing or pending request
    void cancelRequest(uint32_t requestId,
                       const AseUrl & requestUrl,
                       std::vector<IAsyncHttpConnectionPtr> & selectedConnections);

    // Manages requests
    void processNewRequests(const AseTimeVal & timeNow);

    //Processes the HTTPs connections
    void processConnections(std::vector<IAsyncHttpConnectionPtr> const & connections,
                            const AseTimeVal & timeNow);

    bool removePendingRequest(uint32_t id);
    bool isPending(uint32_t id) const;
    bool isActive(uint32_t id) const;
    long getTimeout() const;

private:

    void processConnection(const AseTimeVal & timeNow, Node * pNode);

    void finishRequest(Node * pNode, int32_t nativeCode, long httpStatusCode);

    bool registerConnection(Node * pNode);

    // Allocates a request ID
    uint32_t allocateId();

    //Type definition for node map.
    typedef std::map<uint32_t, NodePtr> NodeMap;

    //The HTTPs request node map.
    NodeMap mNodeMap;

    //The new HTTPs requests map.
    NodeMap mNewNodeMap;

    //The current HTTPs connection count.
    int32_t mConnectionCount;

    //The async HTTP library.
    IAsyncHttpClientPtr mAsyncHttpClientPtr;

    // Notified when a request is finished
    HttpsServiceListener * mListener;

    mutable Mutex mMutex;	// Protects mNewNodeMap

    // Source of request IDs
	static uint32_t mIdPool;

    // Implementation-specific data
    class Impl;
    Impl * mImpl;
};


}


#endif // HTTP_SERVICE_THREAD_H
