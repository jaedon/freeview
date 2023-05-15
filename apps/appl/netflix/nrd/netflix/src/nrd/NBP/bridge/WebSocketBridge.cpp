/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "WebSocketBridge.h"
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Version.h>
#include <nrdbase/MutexStack.h>
#include <nrdnet/WebSocketClient.h>
#include <nrdnet/WebSocketThread.h>

using namespace netflix;

class NrdpWebSocketClient : public WebSocketClient
{
public:
    NrdpWebSocketClient(WebSocketId id, WebSocketBridge *pBridge);
    ~NrdpWebSocketClient();

    WebSocketId getId() const;

    virtual void onOpen(const std::string &url, const std::string &protocol);
    virtual void onMessage(const std::string &data);
    virtual void onBinaryMessage(const std::vector<uint8_t> &data);
    virtual void onError();
    virtual void onClose(int code, const std::string &reason);
    virtual void onMessageQueued();
private:
    WebSocketId mId;
    WebSocketBridge * mpBridge;
};

//////////
//
// properties
//
//////////
#define WebSocketBridgePropertyList(Property)

PropertyTable(WebSocketBridge, WebSocketBridgePropertyList, properties)

//////////
//
// methods
//
//////////
#define WebSocketBridgeMethodList(Method)       \
    Method(close)                               \
    Method(create)                              \
    Method(send)                                \
    Method(test)

MethodTable(WebSocketBridge, WebSocketBridgeMethodList)

//////////
//
// WebSocketBridge
//
//////////
WebSocketBridge::WebSocketBridge()
    : NfObject("websocket",
               Properties::WebSocketBridge::properties, Properties::WebSocketBridge::PropertyCount,
               Methods::WebSocketBridge::methods, Methods::WebSocketBridge::MethodCount),
      mMutex(WEBSOCKETBRIDGE_MUTEX, "WebSocketBridge")
{
}

WebSocketBridge::~WebSocketBridge()
{
    WSTRACE("WebSocketBridge::~WebSocketBridge");

    ScopedMutex lock(mMutex);
    if (mWebSocketThread) {
        mWebSocketThread->stop();
        mWebSocketThread->Wait();
        mWebSocketThread.reset();

        WSTRACE("WebSocketBridge thread ended");
    }
}

Variant WebSocketBridge::invoke(int method, const Variant &var)
{
    Variant result(Variant::Type_StringMap);

    // make sure we pass back the index of the caller
    const std::string index = var.mapValue<std::string>("idx");
    result["idx"] = index;
    result["success"] = false; // assume failure for now

    switch (method) {
#define FORWARD_METHOD(METHODNAME, ...)                                 \
        case Methods::WebSocketBridge::METHODNAME: return invoke_##METHODNAME(result, method, var);

        // Forward all the methods to the same named function
        WebSocketBridgeMethodList(FORWARD_METHOD)
            }

    // should never get here, but pass to default handler
    return NfObject::invoke(method, var);
}

bool WebSocketBridge::getProperty(int index, Variant *result) const
{
    (void)index;
    (void)result;
    return false;
}

// event handlers
void WebSocketBridge::onOpen(WebSocketId websocket, const std::string &url, const std::string &protocol)
{
    WSTRACE("WebSocketBridge::onOpen %d %s %s", websocket, url.c_str(), protocol.c_str());

    Variant event;
    event["type"] = "open";
    event["websocket"] = websocket;
    event["url"] = url;
    event["protocol"] = protocol;
    sendEvent("open", event);
}

void WebSocketBridge::onMessage(WebSocketId websocket, const std::string &data)
{
    WSTRACE("WebSocketBridge::onMessage %d %s", websocket, data.c_str());

    Variant event;
    event["type"] = "message";
    event["websocket"] = websocket;
    event["data"] = data;
    sendEvent("message", event);
}

void WebSocketBridge::onError(WebSocketId websocket)
{
    WSTRACE("WebSocketBridge::onError %d", websocket);

    Variant event;
    event["type"] = "error";
    event["websocket"] = websocket;
    sendEvent("error", event);
}

void WebSocketBridge::onClose(WebSocketId websocket, int code, const std::string &reason)
{
    WSTRACE("WebSocketBridge::onClose %d %d %s", websocket, code, reason.c_str());

    Variant event;
    event["type"] = "close";
    event["websocket"] = websocket;
    event["code"] = code;
    event["reason"] = reason;
    sendEvent("close", event);

    ScopedMutex lock(mMutex);
    ConnectionMap::iterator found = mConnections.find(websocket);

    if (found != mConnections.end()) {
        startWebSocketThread();
        mWebSocketThread->removeConnection(found->second);
        found->second.reset(); // destroys
        mConnections.erase(found);
    }
}

#define READ_VAR(type, name)                    \
    var.mapValue<type>(name, &ok);              \
    do { if (!ok) {                             \
            invalidArgumentError(method, name); \
            sendEvent("result", result);        \
            return false;                       \
        }                                       \
    } while(0)

// invoke handlers
bool WebSocketBridge::invoke_create(Variant &result, int method, const Variant &var)
{
    ScopedMutex lock(mMutex);
    bool ok;
    int websocket = READ_VAR(int, "websocket");
    std::string url = READ_VAR(std::string, "url");
    std::string protocols = READ_VAR(std::string, "protocols");
    /* WebSocketId websocket = 0;*/
    shared_ptr<NrdpWebSocketClient> pClient(new NrdpWebSocketClient(websocket, this));

    if (pClient) {
        websocket = pClient->getId();

        result["success"] = true;
    } else {
        WSERROR("Failed to create WebSocket");
    }

    result["data"]["websocket"] = websocket;
    sendEvent("result", result);

    if (pClient) {
        startWebSocketThread();
        if (pClient->openUrl(url, protocols)) {
            mConnections[websocket] = pClient;
            mWebSocketThread->addConnection(pClient);
        } else {
            onClose(websocket, 0, "Could not connect");
            pClient.reset();
        }
    }

    return true;
}

bool WebSocketBridge::invoke_send(Variant &result, int method, const Variant &var)
{
    ScopedMutex lock(mMutex);
    bool ok;
    WebSocketId websocket = READ_VAR(int, "websocket");
    std::string msg = READ_VAR(std::string, "data");
    ConnectionMap::iterator found = mConnections.find(websocket);

    if (found != mConnections.end()) {
        found->second->queue(msg);

        result["success"] = true;
    } else {
        WSWARN("WebSocket %d not found", websocket);
    }

    sendEvent("result", result);
    return true;
}

bool WebSocketBridge::invoke_close(Variant &result, int method, const Variant &var)
{
    bool ok;
    WebSocketId websocket = READ_VAR(int, "websocket");

    ScopedMutex lock(mMutex);
    ConnectionMap::iterator found = mConnections.find(websocket);
    if (found != mConnections.end()) {
        found->second->closeConnection();
        result["success"] = true;
    } else {
        WSWARN("WebSocket %d not found", websocket);
    }

    sendEvent("result", result);
    return true;
}

bool WebSocketBridge::invoke_test(Variant &result, int method, const Variant &var)
{
    bool ok;
    std::string url = READ_VAR(std::string, "url");

    //runAutobahnClientTest(url);

    result["success"] = true;
    sendEvent("result", result);
    return true;
}

#undef READ_VAR

NrdpWebSocketClient::NrdpWebSocketClient(WebSocketId id, WebSocketBridge *pBridge)
    : WebSocketClient(), mId(id), mpBridge(pBridge)
{
}

NrdpWebSocketClient::~NrdpWebSocketClient()
{
    WSTRACE("NrdpWebSocketClient::~NrdpWebSocketClient");
}

WebSocketId NrdpWebSocketClient::getId() const
{
    return mId;
}

void NrdpWebSocketClient::onOpen(const std::string &url, const std::string &protocol)
{
    WSTRACE("NrdpWebSocketClient::onOpen %s %s", url.c_str(), protocol.c_str());

    if (mpBridge) {
        mpBridge->onOpen(getId(), url, protocol);
    }
}

void NrdpWebSocketClient::onMessage(const std::string &data)
{
    WSTRACE("NrdpWebSocketClient::onMessage %s", data.c_str());

    if (mpBridge) {
        mpBridge->onMessage(getId(), data);
    }
}

void NrdpWebSocketClient::onBinaryMessage(const std::vector<uint8_t> &data)
{
    WSTRACE("NrdpWebSocketClient::onBinaryMessage unimplemented, dropping data");
    (void)data;

    closeConnection();
}

void NrdpWebSocketClient::onError()
{
    WSTRACE("NrdpWebSocketClient::onError");

    if (mpBridge) {
        mpBridge->onError(getId());
    }
}

void NrdpWebSocketClient::onClose(int code, const std::string &reason)
{
    WebSocketClient::onClose(code, reason);

    WSTRACE("NrdpWebSocketClient::onClose %d %s", code, reason.c_str());

    if (mpBridge) {
        mpBridge->onClose(getId(), code, reason);
    }
}

void NrdpWebSocketClient::onMessageQueued()
{
    if (mpBridge) {
        shared_ptr<WebSocketThread> thread = mpBridge->webSocketThread();
        if (thread) {
            thread->wakeup();
        }
    }
}

void WebSocketBridge::startWebSocketThread() // mMutex always held
{
    assert(MutexStack::locked(mMutex.name()));
    if (!mWebSocketThread) {
        mWebSocketThread.reset(new WebSocketThread("WebSocketBridge"));
        mWebSocketThread->Start();
        mWebSocketThread->WaitRunning();
        WSTRACE("WebSocketBridge thread running");
    }
}

shared_ptr<WebSocketThread> WebSocketBridge::webSocketThread() const
{
    ScopedMutex lock(mMutex);
    return mWebSocketThread;
}
