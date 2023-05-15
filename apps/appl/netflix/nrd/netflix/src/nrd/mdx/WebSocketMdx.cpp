/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "WebSocketMdx.h"

#include "MdxLog.h"
#include "MdxInternal.h"
#include "MdxTracing.h"
#include <nrdbase/Variant.h>
#include <nrdnet/WebSocket.h>
#include <nrdnet/WebSocketServer.h>
#include <nrdnet/WebSocketThread.h>

using namespace netflix;
using namespace netflix::mdx;

static ullong sId = 200000000;

MdxServer::MdxServer(const shared_ptr<WebSocketThread> &thread)
    : IWebSocketServer(), mLimit(3000), mThread(thread)
{
    mBuffer.reserve(mLimit);
}

void MdxServer::setMaximumMessageSize(ullong maxSize)
{
    mLimit = maxSize;
    // Note: don't resize the buffer, we might be handling a message
}

bool MdxServer::queue(ullong xid, const std::string &body)
{
    TIMER(__FUNCTION__, "WebSocket");
    EVENT_STEP("Message", xid, "MDX", __FUNCTION__, NULL);

    lock();
    if(mpConnection->isClosed())
    {
        MDXTRACE("MdxServer::queue not queuing while closed");
        unlock();
        return false;
    }

    MDXTRACE("MdxServer::queue %llu", xid);

    Msg msg;
    msg.mXid = xid;
    msg.mBody = body;
    mOutboundMessages.push_back(msg);
    unlock();
    if (shared_ptr<WebSocketThread> thread = mThread.lock())
        thread->wakeup();

    return true;
}

void MdxServer::onOpen(const std::string &url, const std::string &protocol)
{
    (void)url;
    (void)protocol;
}

void MdxServer::onMessageBegin(websocket::MessageType type)
{
    TIMER(__FUNCTION__, "WebSocket");
    mId = sId++;
    EVENT_START("MdxServer", mId, "MDX", __FUNCTION__, NULL);

    lock();

    MDXTRACE("MdxServer::onMessageBegin %s", type == websocket::TextMessage ? "text" : "binary");

    if(type != websocket::TextMessage) {
        MDXWARN("Dropping non text message");
        mpConnection->closeConnection();
        unlock();
        return;
    }

    mMessageRejected = false;

    unlock();
}

void MdxServer::onData(const void *pData, ullong length)
{
    TIMER(__FUNCTION__, "WebSocket");
    EVENT_STEP("MdxServer", mId, "MDX", __FUNCTION__, NULL);

    lock();

    if((mBuffer.length() + length) > mLimit)
    {
        // drop the data, skip the message
        mMessageRejected = true;
    }
    else
    {
        if(length)
        {
            int start = mBuffer.size();
            mBuffer.resize(mBuffer.size() + length);
            memcpy(&mBuffer[start], pData, length);
        }
    }

    unlock();
}

void MdxServer::onMessageEnd()
{
    TIMER(__FUNCTION__, "WebSocket");
    EVENT_STEP("MdxServer", mId, "MDX", __FUNCTION__, NULL);

    if(mMessageRejected)
    {
        MDXERROR("MdxServer::onMessageEnd message was dropped, exceeded limit of %u", mLimit);
    }
    else
    {
        lock();

        MDXTRACE("MdxServer::onMessageEnd: %d bytes", mBuffer.size());

        Variant headers;
        headers["host"] = mpConnection->getHost();
        headers["client"] = mpConnection->getClient();
        headers["subprotocol"] = mpConnection->getSubprotocol();
        std::string body;

        // TODO: send a response?

        // TODO: this doesn't seem right, but this is what MdxGuard does
        body.resize(mBuffer.size() + 1);
        memcpy(&body[0], &mBuffer[0], mBuffer.size());
        body[mBuffer.size()] = '\0';

        mBuffer.clear();

        unlock();

        MdxInternal::getInstance()->callListener(WebSocketMessageEvent(headers, body));
    }

    EVENT_FINISH("MdxServer", mId, "MDX", __FUNCTION__, NULL);
}

void MdxServer::onError()
{
    lock();

    MDXTRACE("MdxServer::onError");

    mBuffer.clear();
    unlock();
}

void MdxServer::onClose(int code, const std::string &reason)
{
    TIMER(__FUNCTION__, "WebSocket");
    lock();
    MDXTRACE("MdxServer::onClose %d %s", code, reason.c_str());

    mBuffer.clear();
    mOutboundMessages.clear();
    unlock();

    IWebSocketServer::onClose(code, reason);
}

bool MdxServer::pendingSend() const
{
    return mOutboundMessages.size() > 0;
}

void MdxServer::onSend()
{
    TIMER(__FUNCTION__, "WebSocket");
    lock();
    std::vector<Msg>::iterator msg = mOutboundMessages.begin();

    while(msg != mOutboundMessages.end())
    {
        ullong xid = msg->mXid;

        EVENT_STEP("Message", xid, "MDX", __FUNCTION__, NULL);

        MDXTRACE("MdxServer::onSend  %llu", xid);

        mpConnection->sendTextMessage(msg->mBody.data(), msg->mBody.length());

        msg = mOutboundMessages.erase(msg);

        MdxInternal::getInstance()->callListener(WebSocketSentEvent(mpConnection->getClient(), xid));
    }

    unlock();
}
