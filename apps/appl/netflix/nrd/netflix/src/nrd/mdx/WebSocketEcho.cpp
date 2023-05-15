/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "WebSocketEcho.h"

using namespace netflix;

class AutobahnTestClient : public WebSocketClient
{
public:
    class IOpenCallback
    {
    public:
        virtual ~IOpenCallback() {}
        virtual void operator() (const std::string &url, const std::string &protocol) = 0;
    };

    class IMessageCallback
    {
    public:
        virtual ~IMessageCallback() {}
        virtual void operator() (const std::string &data) = 0;
    };

    class IBinaryMessageCallback
    {
    public:
        virtual ~IBinaryMessageCallback() {}
        virtual void operator() (const std::vector<uint8_t> &data) = 0;
    };

    class ICloseCallback
    {
    public:
        virtual ~ICloseCallback() {}
        virtual void operator() (int closeCode, const std::string &reason) = 0;
    };

    AutobahnTestClient(IOpenCallback &onOpen,
                       IMessageCallback &onMessage,
                       IBinaryMessageCallback &onBinaryMessage,
                       ICloseCallback &onClose)
        : mOnOpen(onOpen)
        , mOnMessage(onMessage)
        , mOnBinaryMessage(onBinaryMessage)
        , mOnClose(onClose)
    {
    }

    ~AutobahnTestClient()
    {
    }

private:

    void onOpen(const std::string &url, const std::string &protocol)
    {
        mOnOpen(url, protocol);
    }

    void onMessage(const std::string &data)
    {
        mOnMessage(data);
    }

    void onBinaryMessage(const std::vector<uint8_t> &data)
    {
        mOnBinaryMessage(data);
    }

    void onError()
    {
    }

    void onClose(int code, const std::string &reason)
    {
        mOnClose(code, reason);

        WebSocketClient::onClose(code, reason);
    }

    virtual void onMessageQueued()
    {
    }

    IOpenCallback &         mOnOpen;
    IMessageCallback &      mOnMessage;
    IBinaryMessageCallback &mOnBinaryMessage;
    ICloseCallback &        mOnClose;
};

class onTestOpen : public AutobahnTestClient::IOpenCallback
{
public:

    void operator() (const std::string &url, const std::string &protocol)
    {
        NRDP_UNUSED(url);
        NRDP_UNUSED(protocol);
        WSTRACE("TestOpen: %s %s", url.c_str(), protocol.c_str());
    }
};

class onCaseCount : public AutobahnTestClient::IMessageCallback
{
public:
    onCaseCount(int &caseCount) : mCaseCount(caseCount) {}

    void operator() (const std::string &data)
    {
        WSTRACE("CaseCount: %s", data.c_str());
        sscanf(data.c_str(), " %d", &mCaseCount);
    }

private:

    int &mCaseCount;
};

class onTestMessage : public AutobahnTestClient::IMessageCallback
{
public:

    void operator() (const std::string &data)
    {
        NRDP_UNUSED(data);
        WSTRACE("TestMessage: %s", data.c_str());
    }
};

class onTestBinaryMessage : public AutobahnTestClient::IBinaryMessageCallback
{
public:

    void operator() (const std::vector<uint8_t> &data)
    {
        NRDP_UNUSED(data);
        WSTRACE("TestBinaryMessage: %zu bytes", data.size());
    }
};

class onTestClose : public AutobahnTestClient::ICloseCallback
{
public:

    void operator() (int closeCode, const std::string &reason)
    {
        NRDP_UNUSED(closeCode);
        NRDP_UNUSED(reason);
        WSTRACE("TestClose: %d %s", closeCode, reason.c_str());
    }
};

void runAutobahnClientTest(const std::string &url)
{
    int caseCount = 0;
    websocket::ResultType result;

    onTestOpen          onOpen;
    onCaseCount         onCaseCount(caseCount);
    onTestMessage       onMessage;
    onTestBinaryMessage onBinaryMessage;
    onTestClose         onClose;

    AutobahnTestClient getCaseCount(onOpen, onCaseCount, onBinaryMessage, onClose);
    EchoClient echoClient;

    if(getCaseCount.openUrl(url + "/getCaseCount", ""))
    {
        do
        {
            result = getCaseCount.mpConnection->process();
        } while(!getCaseCount.mpConnection->isClosed() && (result == websocket::Continue || result == websocket::Success));
    }

    WSTRACE("Will run %d tests", caseCount);

    for(int i = 0; i < caseCount; i++)
    {
        char caseUrl[128];
        snprintf(caseUrl, sizeof(caseUrl), "/runCase?case=%d&agent=%s", i + 1, "NRDPCPP");

        if(echoClient.openUrl(url + caseUrl, ""))
        {
            do
            {
                result = echoClient.mpConnection->process();
            } while(!echoClient.mpConnection->isClosed() && (result == websocket::Continue || result == websocket::Success));
        }
    }

    AutobahnTestClient updateReports(onOpen, onMessage, onBinaryMessage, onClose);

    if(updateReports.openUrl(url + "/updateReports?agent=NRDPCPP", ""))
    {
        do
        {
            result = updateReports.mpConnection->process();
        } while(!updateReports.mpConnection->isClosed() && (result == websocket::Continue || result == websocket::Success));
    }

    WSTRACE("Autobahn client tests done");
}
