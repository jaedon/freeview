/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __WEBSOCKETECHO_H__
#define __WEBSOCKETECHO_H__

#include <nrdnet/WebSocket.h>
#include <nrdnet/WebSocketClient.h>
#include <nrdnet/WebSocketServer.h>

namespace netflix
{
// Right now, the interface of a server and the
// interface of a client are practically the same
template <typename Base>
class EchoImpl : public Base
{
public:
    EchoImpl();
    virtual ~EchoImpl();
    void onOpen(const std::string &url, const std::string &protocol);
    void onMessageBegin(websocket::MessageType type);
    void onData(const void *pData, ullong length);
    void onMessageEnd();
    void onError();
    void onClose(int code, const std::string &reason);

    bool pendingSend() const;
    void onSend();
private:
    websocket::MessageType mMessageType;
    std::vector<uint8_t> mBuffer;
    bool mFirst;
};

typedef EchoImpl<IWebSocketServer> EchoServer;
typedef EchoImpl<IWebSocketClient> EchoClient;

template <typename Base>
inline EchoImpl<Base>::EchoImpl()
    : mMessageType(websocket::TextMessage), mFirst(true)
{
}

template <typename Base>
inline EchoImpl<Base>::~EchoImpl()
{
}

template <typename Base>
inline void EchoImpl<Base>::onOpen(const std::string &url, const std::string &protocol)
{
    WSTRACE("EchoImpl::onOpen %s %s", url.c_str(), protocol.c_str());
    NRDP_UNUSED(url);
    NRDP_UNUSED(protocol);
}

template <typename Base>
inline void EchoImpl<Base>::onMessageBegin(websocket::MessageType type)
{
    WSTRACE("EchoImpl::onMessageBegin %s", type == websocket::TextMessage ? "text" : "binary");

    mMessageType = type;
    mBuffer.clear();
    mFirst = true;
}

template <typename Base>
inline void EchoImpl<Base>::onData(const void *pData, ullong length)
{
    for(size_t i = 0; i < length; i++)
    {
        mBuffer.push_back(*((const uint8_t *)pData + i));

        if(mBuffer.size() > 64 * 1024)
        {
            if(mFirst)
            {
                WSTRACE("EchoImpl::onData Sending first %zu bytes", mBuffer.size());

                Base::mpConnection->beginMessage(mMessageType, &mBuffer[0], mBuffer.size());
                mBuffer.clear();
                mFirst = false;
            }
            else
            {
                WSTRACE("EchoImpl::onData Flushing %zu bytes", mBuffer.size());

                Base::mpConnection->continueMessage(&mBuffer[0], mBuffer.size());
                mBuffer.clear();
            }
        }
    }
}

template <typename Base>
inline void EchoImpl<Base>::onMessageEnd()
{
    WSTRACE("EchoImpl::onMessageEnd");

    if(mFirst)
    {
        WSTRACE("EchoImpl::onMessageEnd Sending all %zu bytes", mBuffer.size());

        if(mMessageType == websocket::TextMessage)
        {
            Base::mpConnection->sendTextMessage(&mBuffer[0], mBuffer.size());
        }
        else
        {
            Base::mpConnection->sendBinaryMessage(&mBuffer[0], mBuffer.size());
        }

        mBuffer.clear();
        mFirst = false;
    }
    else
    {
        WSTRACE("EchoImpl::onMessageEnd Sending last %zu bytes", mBuffer.size());

        Base::mpConnection->endMessage(&mBuffer[0], mBuffer.size());
        mBuffer.clear();
    }
}

template <typename Base>
inline void EchoImpl<Base>::onError()
{
    WSTRACE("EchoImpl::onError");

    mBuffer.clear();
}

template <typename Base>
inline void EchoImpl<Base>::onClose(int code, const std::string &reason)
{
    NRDP_UNUSED(code);
    NRDP_UNUSED(reason);
    WSTRACE("EchoImpl::onClose %d %s", code, reason.c_str());

    mBuffer.clear();
    Base::onClose(code, reason);
}

template <typename Base>
inline bool EchoImpl<Base>::pendingSend() const
{
    return false;
}

template <typename Base>
inline void EchoImpl<Base>::onSend()
{
}

void runAutobahnClientTest(const std::string &url);
}

#endif // __WEBSOCKETECHO_H__
