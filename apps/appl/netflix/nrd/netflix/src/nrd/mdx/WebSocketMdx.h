/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __WEBSOCKETMDX_H__
#define __WEBSOCKETMDX_H__

#include <nrdnet/WebSocketServer.h>

namespace netflix {
class WebSocketThread;
namespace mdx {

class MdxServer : public IWebSocketServer
{
public:

    MdxServer(const shared_ptr<WebSocketThread> &thread);

    void setMaximumMessageSize(ullong maxSize);

    bool queue(ullong xid, const std::string &body);

protected:

    virtual void onOpen(const std::string &url, const std::string &protocol);
    virtual void onMessageBegin(websocket::MessageType type);
    virtual void onData(const void *pData, ullong length);
    virtual void onMessageEnd();
    virtual void onError();
    virtual void onClose(int code, const std::string &reason);

    virtual bool pendingSend() const;
    virtual void onSend();

private:
    ullong mLimit;
    ullong mId;
    std::string mBuffer;
    bool mMessageRejected;

    struct Msg
    {
        ullong mXid;
        std::string mBody;
    };

    std::vector<Msg> mOutboundMessages;
    weak_ptr<WebSocketThread> mThread;
};

}}

#endif // __WEBSOCKETMDX_H__

