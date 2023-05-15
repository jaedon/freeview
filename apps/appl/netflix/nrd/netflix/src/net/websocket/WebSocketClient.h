/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __WEBSOCKETCLIENT_H__
#define __WEBSOCKETCLIENT_H__

#include "WebSocket.h"
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Version.h>
#include <nrdnet/IAsyncHttpLib.h>

namespace netflix {

class IWebSocketClient : public IWebSocketHandler
{
public:

    IWebSocketClient();
    virtual ~IWebSocketClient();

    websocket::ResultType   open(websocket::socket sock,
                                 const std::string &url,
                                 const std::string &subprotocols,
                                 const std::vector<uint8_t> &key,
                                 const HttpResponseHeaders &responseHeaders);

    bool                    openUrl(const std::string &url, const std::string &protocols);

    void                    closeConnection();

    virtual void            onClose(int code, const std::string &reason);
    virtual void            waitForClose();

protected:

    void lock();
    void unlock();

private:

    Mutex             mMutex;
    bool                    mIsClosed;
    ConditionVariable mClosedCondition;

    shared_ptr<IAsyncHttpClient>       m_httpClient;
    shared_ptr<IAsyncHttpConnection>   m_httpConnection;
    shared_ptr<IAsyncHttpRequest>      m_httpRequest;

};

// Default implementation
class WebSocketClient : public IWebSocketClient
{
public:

    WebSocketClient();

    bool queue(const std::string &body);

    virtual void onMessage(const std::string &message) = 0;
    virtual void onBinaryMessage(const std::vector<uint8_t> &message) = 0;
    virtual void onError();
    virtual void onClose(int code, const std::string &reason);

protected:

    // from IWebSocketHandler
    virtual void onMessageBegin(websocket::MessageType type);
    virtual void onData(const void *pData, ullong length);
    virtual void onMessageEnd();

    virtual bool pendingSend() const;
    virtual void onSend();

    virtual void onMessageQueued() = 0;
private:

    struct Msg
    {
        std::string mBody;
    };

    websocket::MessageType  mMessageType;
    std::string             mBuffer;
    std::vector<Msg>        mOutboundMessages;

};

} // namespace netflix::net

#endif // __WEBSOCKETCLIENT_H__
