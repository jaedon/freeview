/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "WebSocketClient.h"
#include "WebSocket.h"
#include <nrdbase/Log.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/Random.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Variant.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/Version.h>

#include <nrdbase/ASError.h>
#include <nrdbase/Base64.h>
#include <nrdbase/Version.h>
#include <nrdnet/AseUrl.h>
#include <nrdnet/IAsyncHttpLib.h>
#include <nrdnet/IDnsCache.h>
#include <nrdnet/IpConnectivityManager.h>

using namespace netflix;

NRDP_DEFINE_OBJECTCOUNT(IWebSocketClient);

#define SOCKET_BUFFER     (32 * 1024)

IWebSocketClient::IWebSocketClient()
    : mMutex(MDXWEBSOCKETCLIENT_MUTEX, "WebSocketClient")
    , mIsClosed(true)
{
    NRDP_OBJECTCOUNT_REF(IWebSocketClient, this);
}

IWebSocketClient::~IWebSocketClient()
{
    NRDP_OBJECTCOUNT_DEREF(IWebSocketClient, this);
}

websocket::ResultType IWebSocketClient::open(websocket::socket sock,
                                             const std::string &url,
                                             const std::string &protocols,
                                             const std::vector<uint8_t> &key,
                                             const HttpResponseHeaders &responseHeaders)
{
    ScopedMutex lock(mMutex);

    websocket::ConnectionConfig config;
    websocket::defaultConfig(config);
    config.mSendLimit = 1 * 1024 * 1024;

    websocket::ResultType result = websocket::handshakeResponse(sock,
                                                                url,
                                                                protocols,
                                                                key,
                                                                responseHeaders,
                                                                "localHost",
                                                                "remoteHost",
                                                                config,
                                                                this);

    if(result != websocket::Success)
    {
        WSWARN("IWebSocketClient::open Failed handshake");
        return result;
    }

    WSTRACE("IWebSocketClient::open handshake success");

    if(!mpConnection)
    {
        WSWARN("IWebSocketClient::open No connection made?");
        return websocket::NoMemory;
    }

    mIsClosed = false;

    // good to go, proceed with WebSocket protocol traffic
    onOpen(mpConnection->getUrl(), mpConnection->getSubprotocol());

    return websocket::Success;
}

bool IWebSocketClient::openUrl(const std::string &url, const std::string &protocols)
{
    WSTRACE("WebSocketClient::openUrl Opening http connection to %s", url.c_str());

    // construct the http client
    int32_t aserr = constructAsyncHttpClient(m_httpClient,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL);
    if(aserr != AS_NO_ERROR)
    {
        WSWARN("WebSocketClient::openUrl Failed to construt http client: %d", aserr);

        return false;
    }

    aserr = m_httpClient->init();
    if(aserr != AS_NO_ERROR)
    {
        WSWARN("WebSocketClient::openUrl Failed to init http client: %d", aserr);

        // delete the http client as it failed to init
        m_httpClient->deinit();
        m_httpClient.reset();

        return false;
    }

    AseUrl aseUrl(url.c_str());

    aserr = m_httpClient->openConnection(aseUrl.GetHost(),
                                         aseUrl.GetPortNumber(),
                                         AseTimeVal::ZERO,
                                         AseTimeVal::ZERO,
                                         SOCKET_BUFFER,
                                         NULL,
                                         true,
                                         /*out*/m_httpConnection);
    if((aserr != AS_NO_ERROR) && (aserr != AS_IN_PROGRESS))
    {
        WSWARN("WebSocketClient::openUrl Failed to open connection: %d", aserr);
        m_httpClient->deinit();
        m_httpClient.reset();
        return false;
    }

    std::vector<unsigned char> key = Random::next(16);

    do
    {
        // wait for client to be ready
        std::vector<shared_ptr<IAsyncHttpConnection> > connections;
        aserr = m_httpClient->select(/*out*/connections, AseTimeVal::fromMS(100));
        if (aserr == AS_NO_ERROR)
        {
            // timed out, loop back abd check for abort
            continue;
        }
        if (aserr != IAsyncHttpClient::SELECT_CONNECT && aserr != IAsyncHttpClient::SELECT_WAKE_AND_CONNECT)
        {
            // select failed
            WSWARN("WebSocketClient::openUrl select failed: %d", aserr);
            m_httpClient->deinit();
            m_httpClient.reset();
            return false;
        }

        // connection event
        IAsyncHttpConnection::Event event = m_httpConnection->retrieveOutstandingEvent();
        if (event == IAsyncHttpConnection::STATE_CHANGE_EVENT)
        {
            if (m_httpConnection->getState() == IAsyncHttpConnection::IDLE_STATE)
            {
                HttpHeaders headers;

                websocket::buildClientHandshake(key, protocols, headers);

                // issue request
                aserr = m_httpConnection->issueGetRequest(aseUrl, headers, IHttpRequestTraceListenerPtr(),
                                                          /*out*/m_httpRequest);
                if (aserr != AS_NO_ERROR)
                {
                    WSWARN("WebSocketClient::openUrl HTTP GET failed: %d", aserr);
                    m_httpClient->deinit();
                    m_httpClient.reset();
                    return false;
                }
            }
            else
            {
                // failed to open the connection
                aserr = m_httpConnection->getConnectionFailureCode();
                WSWARN("WebSocketClient::openUrl Failed to open connection: %d", aserr);
                m_httpClient->deinit();
                m_httpClient.reset();
                return false;
            }
        }
        else if (event == IAsyncHttpConnection::RESPONSE_AVAILABLE_EVENT)
        {
            IAsyncHttpRequestPtr pTriggeringHttpRequest;

            pTriggeringHttpRequest = m_httpConnection->getTriggeringRequest();

            assert( m_httpRequest->getId() == pTriggeringHttpRequest->getId() );

            IAsyncHttpRequest::State requestState = m_httpRequest->getState();
            if ((requestState == IAsyncHttpRequest::RECEIVING_BODY_STATE) ||
                (requestState == IAsyncHttpRequest::COMPLETED_STATE))
            {
                // check http response code
                uint32_t code = m_httpRequest->getResponseCode();

                // we only support an upgrade
                if (code != 101)
                {
                    WSWARN("WebSocketClient::openUrl HTTP response code %d", code);
                    m_httpClient->deinit();
                    m_httpClient.reset();
                    return false;
                }

                // request ready for handoff to WebSocket
                int32_t sock = m_httpConnection->transferSocketHandle();

                open(sock, url, protocols, key, m_httpRequest->getResponseHeaders());
                break;
            }
        }
    } while(1);

    return true;
}

void IWebSocketClient::closeConnection()
{
    ScopedMutex lock(mMutex);

    if(!mIsClosed) {
        if(mpConnection) {
            mpConnection->closeConnection();
        } else {
            onClose(0, "");
        }
    }
}

void IWebSocketClient::onClose(int code, const std::string &reason)
{
    (void)code;
    (void)reason;

    lock();
    WSTRACE("IWebSocketClient::onClose %d %s", code, reason.c_str());


    if (m_httpConnection)
    {
        m_httpClient->closeConnection(m_httpConnection);
        m_httpConnection.reset();
    }
    if (m_httpClient)
    {
        m_httpClient->deinit();
        m_httpClient.reset();
    }

    if (!mpConnection->closeSocket()) {
        WSTRACE("IWebSocketClient::onClose trying to close already closed socket");
    }

    mIsClosed = true;
    mClosedCondition.broadcast();

    unlock();
}

void IWebSocketClient::waitForClose()
{
    ScopedMutex lock(mMutex);

    if(!mIsClosed) {
        mClosedCondition.wait(&mMutex);
    }
}

void IWebSocketClient::lock()
{
    mMutex.lock();
}

void IWebSocketClient::unlock()
{
    mMutex.unlock();
}

WebSocketClient::WebSocketClient()
{
}

bool WebSocketClient::queue(const std::string &body)
{
    lock();
    if(!mpConnection || mpConnection->isClosed())
    {
        WSTRACE("WebSocketClient::queue not queuing while closed");
        unlock();
        return false;
    }

    WSTRACE("WebSocketClient::queue");

    Msg msg;
    msg.mBody = body;
    mOutboundMessages.push_back(msg);
    unlock();
    onMessageQueued();

    return true;
}

void WebSocketClient::onMessageBegin(websocket::MessageType type)
{
    lock();

    WSTRACE("WebSocketClient::onMessageBegin %s", type == websocket::TextMessage ? "text" : "binary");

    mMessageType = type;
    unlock();
}

void WebSocketClient::onData(const void *pData, ullong length)
{
    lock();

    if(length)
    {
        int start = mBuffer.size();
        mBuffer.resize(mBuffer.size() + length);
        memcpy(&mBuffer[start], pData, length);
    }

    unlock();
}

void WebSocketClient::onMessageEnd()
{
    lock();

    WSVERBOSE("WebSocketCient::onMessageEnd %d bytes", mBuffer.size());

    if(mMessageType == websocket::TextMessage)
    {
        std::string body(mBuffer.begin(), mBuffer.end());

        onMessage(body);
    }
    else if(mMessageType == websocket::BinaryMessage)
    {
        std::vector<uint8_t> body(mBuffer.begin(), mBuffer.end());

        onBinaryMessage(body);
    }

    mBuffer.clear();

    unlock();
}

void WebSocketClient::onError()
{
    lock();

    WSTRACE("WebSocketClient::onError");

    mBuffer.clear();
    unlock();
}

void WebSocketClient::onClose(int code, const std::string &reason)
{
    lock();
    WSTRACE("WebSocketClient::onClose %d %s", code, reason.c_str());

    mBuffer.clear();
    mOutboundMessages.clear();

    unlock();

    IWebSocketClient::onClose(code, reason);
}

bool WebSocketClient::pendingSend() const
{
    return mOutboundMessages.size() > 0;
}

void WebSocketClient::onSend()
{
    lock();
    std::vector<Msg> sendingMessages(mOutboundMessages.begin(), mOutboundMessages.end());
    mOutboundMessages.clear();
    unlock();

    std::vector<Msg>::iterator msg = sendingMessages.begin();

    while(msg != sendingMessages.end())
    {
        WSTRACE("WebSocketClient::onSend %zu '%s'", msg->mBody.length(), msg->mBody.c_str());

        mpConnection->sendTextMessage(msg->mBody.data(), msg->mBody.length());

        msg = sendingMessages.erase(msg);
    }
}
