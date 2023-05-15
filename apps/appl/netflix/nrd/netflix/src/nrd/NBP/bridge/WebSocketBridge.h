/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __WEBSOCKETBRIDGE_H__
#define __WEBSOCKETBRIDGE_H__

#include <nrd/NfObject.h>
#include <nrdbase/Variant.h>

class NrdpWebSocketClient;
typedef int WebSocketId;

namespace netflix {
class WebSocketThread;


class WebSocketBridge : public NfObject
{
public:
    WebSocketBridge();
    virtual ~WebSocketBridge();

    virtual Variant invoke(int method, const Variant &data);
    virtual bool getProperty(int index, Variant *result) const;

    // event handlers
    void onOpen(WebSocketId websocket, const std::string &url, const std::string &protocol);
    void onMessage(WebSocketId websocket, const std::string &data);
    void onError(WebSocketId websocket);
    void onClose(WebSocketId websocket, int code, const std::string &reason);

    shared_ptr<WebSocketThread> webSocketThread() const;

private:
    void startWebSocketThread();
    typedef std::map<WebSocketId, shared_ptr<NrdpWebSocketClient> >  ConnectionMap;

    // invoke handlers
    bool invoke_create(Variant &result, int method, const Variant &var);
    bool invoke_send(Variant &result, int method, const Variant &var);
    bool invoke_close(Variant &result, int method, const Variant &var);
    bool invoke_test(Variant &result, int method, const Variant &var);

    mutable Mutex mMutex;
    ConnectionMap mConnections;
    shared_ptr<WebSocketThread> mWebSocketThread;
};

}
#endif

