/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxMongooseNFLX.h"
#include "MdxLog.h"
#include <nrdbase/JSONFormatter.h>
#include <nrdbase/Variant.h>
#include <nrdbase/ConfigData.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/UrlEncoder.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Version.h>
#include <nrdbase/Log.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/Version.h>
#include <nrdnet/WebSocket.h>
#include <nrdnet/WebSocketThread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "MdxGuard.h"
#include "MdxWebSocketConnections.h"
#include "MdxTracing.h"
#include "MdxUtil.h"
#include "WebSocketMdx.h"
#include "WebSocketEcho.h"

using namespace netflix;
using namespace netflix::mdx;

#ifdef BUILD_DEBUG
#define PRETTY
#if 0
#  define DEBUG_REQUESTS 2
# endif
#endif

#define DEFAULT_MAX_POST_SIZE 3300
#define ABSOLUTE_MAX_POST_SIZE (100 * 1024)
#define WEBSOCKET_RECVBUFFER_SIZE 4096
#define TRACE_EVENT_ID_START ("10000")

MdxMongooseNFLX* MdxMongooseNFLX::sInstance = 0;

MdxMongooseNFLX::MdxMongooseNFLX()
    : mMongoose(NULL), mMaximumPostSizeBytes(DEFAULT_MAX_POST_SIZE), mListeningPort(0)
{
    assert(!sInstance);
    sInstance = this;
}

MdxMongooseNFLX::~MdxMongooseNFLX()
{
    assert(sInstance == this);
    sInstance = NULL;
}

MdxMongooseNFLX* MdxMongooseNFLX::instance()
{
    return sInstance;
}

static int mdxMongooseBeginRequest(struct nflx_mg_connection *conn)
{
    if(MdxMongooseNFLX::instance())
    {
        return MdxMongooseNFLX::instance()->mdxMongooseBeginRequest(conn);
    }

    return 0;
}

static void mdxMongooseEndRequest(const struct nflx_mg_connection *conn, int reply_status_code)
{
    if(MdxMongooseNFLX::instance())
    {
        MdxMongooseNFLX::instance()->mdxMongooseEndRequest(conn, reply_status_code);
    }
}

static int mdxMongooseLogMessage(const struct nflx_mg_connection *conn, const char *message)
{
    if(MdxMongooseNFLX::instance())
    {
        return MdxMongooseNFLX::instance()->mdxMongooseLogMessage(conn, message);
    }

    return 0;
}

#ifdef MDXLIB_HAVE_TRACING
static void mdxMongooseTraceEventStart(int id, const char *message)
{
    (void)id;
    (void)message;
    if(MdxMongooseNFLX::instance())
    {
        MdxMongooseNFLX::instance()->mdxMongooseTraceEventStart(id, message);
    }
}

static void mdxMongooseTraceEventEnd(int id, const char *message)
{
    (void)id;
    (void)message;
    if(MdxMongooseNFLX::instance())
    {
        MdxMongooseNFLX::instance()->mdxMongooseTraceEventEnd(id, message);
    }
}
#endif

void MdxMongooseNFLX::stop()
{
    MdxWebSocketConnections::clearConnections();

    if (mWebSocketThread) {
        mWebSocketThread->stop();
        mWebSocketThread->Wait(); // blocks
        mWebSocketThread.reset();
        NTRACE(TRACE_WEBSOCKET, "WebSocket thread ended");
    }

    nflx_mg_stop(mMongoose); // blocks
    mMongoose = NULL;
}

bool MdxMongooseNFLX::isRunning() const
{
    return mMongoose ? true : false;
}

void MdxMongooseNFLX::setMaximumPostSize(unsigned int maxPostSizeBytes)
{
    if(maxPostSizeBytes <= ABSOLUTE_MAX_POST_SIZE)
    {
        mMaximumPostSizeBytes = maxPostSizeBytes;
    }
    else
    {
        MDXWARN("maxPostSizeBytes exceeds absolut max of %u by %d", ABSOLUTE_MAX_POST_SIZE, maxPostSizeBytes - ABSOLUTE_MAX_POST_SIZE);
    }
}

unsigned int MdxMongooseNFLX::getMaximumPostSize() const
{
    return mMaximumPostSizeBytes;
}

int MdxMongooseNFLX::getListeningPort() const
{
    return mListeningPort;
}

bool MdxMongooseNFLX::start(int listeningPort, bool enableWebSocket)
{
    assert(!mMongoose);
    std::string objName("nrdp.mdx");

    mListeningPort = listeningPort;

    std::stringstream ss;
    std::string listeningPorts;
    std::string maxRequestSize;

    ss << listeningPort << " " << mMaximumPostSizeBytes + 1024;

    ss >> listeningPorts;
    ss >> maxRequestSize;

    std::map<std::string, std::string> configs;
    configs["enable_directory_listing"] = "no";
    configs["num_threads"] = "5";
    //   configs["max_threads"] = "10";
    configs["max_request_size"] = maxRequestSize;
    configs["listening_ports"] = listeningPorts;
    configs["trace_event_id_start"] = TRACE_EVENT_ID_START;

    const Variant &values = Configuration::configDataValues();
    if (values.contains("mdx_rest_port")) {
        configs["listening_ports"] = values.mapValue<std::string>("mdx_rest_port");
        mListeningPort = values.mapValue<int>("mdx_rest_port");
        const Variant::StringMapIterator end = values.stringMapEnd();
        for (Variant::StringMapIterator it = values.stringMapBegin(); it != end; ++it) {
            const std::string &key = (*it).first;
            if (key.size() > 3 && !key.compare(0, 3, "nflx_mg_") && (*it).second.isString())
                configs[key.substr(3)] = (*it).second.string();
        }
    }

    std::vector<const char*> mongooseOptions;
    mongooseOptions.reserve((configs.size() * 2) + 1);
    for (std::map<std::string, std::string>::const_iterator it = configs.begin(); it != configs.end(); ++it)
    {
        mongooseOptions.push_back((*it).first.c_str());
        mongooseOptions.push_back((*it).second.c_str());
        NTRACE(TRACE_MDX_MONGOOSE, "MdxMongooseNFLX option: %s = %s", (*it).first.c_str(), (*it).second.c_str());
    }
    mongooseOptions.push_back(0);

    nflx_mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.begin_request = ::mdxMongooseBeginRequest;
    callbacks.end_request = ::mdxMongooseEndRequest;
    callbacks.log_message = ::mdxMongooseLogMessage;
#ifdef MDXLIB_HAVE_TRACING
    callbacks.trace_event_start = ::mdxMongooseTraceEventStart;
    callbacks.trace_event_end = ::mdxMongooseTraceEventEnd;
#endif
    mMongoose = nflx_mg_start(&callbacks, this, &mongooseOptions[0]);

    if (!mMongoose)
    {
        Log::fatal(
            TRACE_MDX_MONGOOSE,
            "Can't start MDX mongoose - Check that there is not another instance of the application running");
        return false;
    }

    if (enableWebSocket) {
        mWebSocketThread.reset(new WebSocketThread(std::string("Mdx")));
        mWebSocketThread->Start();
        mWebSocketThread->WaitRunning();
    }

    return true;
}

static inline bool readPostData(struct nflx_mg_connection *conn, unsigned int maxPostSizeBytes, std::string &postData)
{
    const char *contentLengthString = nflx_mg_get_header(conn, "Content-Length");
    const unsigned int contentLength = (contentLengthString ? strtoull(contentLengthString, 0, 10) : 0);

    if (contentLength == 0)
    {
        postData = "";
        return true;
    }
    else if (contentLength <= maxPostSizeBytes)
    {
        postData.resize(contentLength + 1);
        const int read = nflx_mg_read(conn, &postData[0], contentLength);
        assert(read >= 0 && (unsigned int)read <= contentLength);
        postData[read] = '\0';
        return true;
    }
    NTRACE(TRACE_MDX_MONGOOSE, "MdxMongooseNFLX: dropping message that exceeds %u bytes", maxPostSizeBytes);
    return false;
}

void MdxMongooseNFLX::handlePing(struct nflx_mg_connection *conn)
{
    writeDataF(conn,
               "HTTP/1.1 200 OK\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "Cache: no-cache\r\n"
               "Content-Type: text/plain\r\n"
               "Connection: close\r\n"
               "Date: %s\r\n"
               "Content-Length: 4\r\n\r\n"
               "pong", Time::now().calendarTimeText().c_str());
}

bool MdxMongooseNFLX::handleREST(struct nflx_mg_connection *conn,
                                 const struct nflx_mg_request_info* requestInfo)
{
    TIMER(__FUNCTION__, "Mongoose");

    Variant headers;
    std::string requestData = "";
    std::string path = requestInfo->uri ? requestInfo->uri : "";

    if (strcmp(requestInfo->request_method, "POST") == 0)
    {
        if(!readPostData(conn, mMaximumPostSizeBytes, requestData))
        {
            // NOTE: we return true here because we handled this message,
            // we just decided to drop it because it exceeds the specified
            // maximum for post data
            return true;
        }
    } else {
        if (requestInfo->query_string) requestData = std::string(requestInfo->query_string);
    }
    for (int i=0; i < requestInfo->num_headers; i++) {
        if (requestInfo->http_headers[i].name && requestInfo->http_headers[i].value) {
            headers[std::string(requestInfo->http_headers[i].name)] = std::string(requestInfo->http_headers[i].value);
            NTRACE(TRACE_MDX_MONGOOSE, "MdxMongooseNFLX header: [ %s:%s ]", requestInfo->http_headers[i].name,
                   requestInfo->http_headers[i].value);
        }
    }

    NTRACE(TRACE_MDX_MONGOOSE,
           "MdxMongooseNFLX IP=%d.%d.%d.%d port=%d path: [%s]",
           (uint8_t)((requestInfo->remote_ip >> 24) & 0xFF),
           (uint8_t)((requestInfo->remote_ip >> 16) & 0xFF),
           (uint8_t)((requestInfo->remote_ip >> 8) & 0xFF),
           (uint8_t)((requestInfo->remote_ip) & 0xFF),
           requestInfo->remote_port,
           path.c_str());
    NTRACE(TRACE_MDX_MONGOOSE, "MdxMongooseNFLX requestData: [%s]", requestData.c_str());

    MdxGuard::processIncomingRequest(path, headers, requestData, (void *)conn);

    return true;
}

void MdxMongooseNFLX::runEchoServer(const websocket::ConnectionInfo &connectionInfo, const std::string &subprotocol)
{
    shared_ptr<EchoServer> pServer(new EchoServer);
    websocket::ConnectionConfig config;

    websocket::defaultConfig(config);
    config.mRecvBufferSize = WEBSOCKET_RECVBUFFER_SIZE;
    config.mSendLimit = 1 * 1024 * 1024;

    if(websocket::Success == websocket::acceptConnection(connectionInfo, subprotocol, config, pServer.get()))
    {
        mWebSocketThread->addConnection(pServer);

        // Block this thread, so that Mongoose does not close the socket
        pServer->waitForClose();

        mWebSocketThread->removeConnection(pServer);
    }
}

void MdxMongooseNFLX::runMdxServer(const websocket::ConnectionInfo &connectionInfo, const std::string &subprotocol)
{
    shared_ptr<MdxServer> pServer = MdxWebSocketConnections::getConnection(connectionInfo.mClient);
    websocket::ConnectionConfig config;

    // MDX only allows one connection at a time from the same client
    if(pServer.get())
    {
        NTRACE(TRACE_WEBSOCKET, "Closing previous connection from %s, only one "
               "concurrent MDX connection from a client at once",
               connectionInfo.mClient.c_str());

        MdxWebSocketConnections::removeConnection(connectionInfo.mClient, pServer);
        pServer->closeConnection();
        pServer->waitForClose();
    }

    pServer.reset(new MdxServer(mWebSocketThread));

    websocket::defaultConfig(config);
    config.mRecvBufferSize = WEBSOCKET_RECVBUFFER_SIZE;

    if(pServer.get() &&
       websocket::Success == websocket::acceptConnection(connectionInfo, subprotocol, config, pServer.get()))
    {
        // if accept is successful, track the connection
        if(MdxWebSocketConnections::addConnection(connectionInfo.mClient, pServer))
        {
            // Let the WebSocketThread handle the communication
            mWebSocketThread->addConnection(pServer);

            pServer->setMaximumMessageSize(mMaximumPostSizeBytes);

            // Block this thread, so that Mongoose does not close the socket
            pServer->waitForClose();

            // This connection is done, remove it from the WebSocketThread
            mWebSocketThread->removeConnection(pServer);

            // Remove it from the tracked connections
            MdxWebSocketConnections::removeConnection(connectionInfo.mClient, pServer);

            return;
        }
    }

    Log::error(TRACE_WEBSOCKET, "Could not start MDX websocket server for client %s (%s)", connectionInfo.mClient.c_str(), subprotocol.c_str());

    // we couldn't service this connection
    websocket::rejectConnection(connectionInfo);
}

bool MdxMongooseNFLX::handleWebSocket(struct nflx_mg_connection *conn, const struct nflx_mg_request_info *requestInfo)
{
    WSTRACE("begin handleWebSocket");

    int clientSocket = nflx_mg_get_socket(conn);

    HttpHeaders requestHeaders;
    for(int i = 0; i < requestInfo->num_headers; i++)
    {
        // TODO: Does this mean that you cannot have multiple headers with the same name? I think HTTP allows that...
        requestHeaders[requestInfo->http_headers[i].name] = requestInfo->http_headers[i].value;
    }

    char addrBuf[INET_ADDRSTRLEN + 16];

    sprintf(addrBuf, "%d.%d.%d.%d",
            (uint8_t)((requestInfo->remote_ip >> 24) & 0xFF),
            (uint8_t)((requestInfo->remote_ip >> 16) & 0xFF),
            (uint8_t)((requestInfo->remote_ip >> 8) & 0xFF),
            (uint8_t)((requestInfo->remote_ip) & 0xFF));

    websocket::ConnectionInfo connectionInfo;

    if(websocket::Success != websocket::handshake(clientSocket,
                                                  requestInfo->uri ? requestInfo->uri : "",
                                                  requestHeaders,
                                                  addrBuf,
                                                  connectionInfo))
    {
        WSTRACE("WebSocket connection failed handshake");
    }
    else
    {
        if(connectionInfo.mSubprotocols.size() == 0)
        {
            WSTRACE("Handled by EchoServer");
            runEchoServer(connectionInfo, "");
        }
        else
        {
            std::vector<std::string>::const_iterator i;

            for(i = connectionInfo.mSubprotocols.begin(); i != connectionInfo.mSubprotocols.end(); ++i)
            {
                WSTRACE("Protocol: '%s'", (*i).c_str());

                if(*i == "echo")
                {
                    WSTRACE("Handled by EchoServer");
                    runEchoServer(connectionInfo, "echo");
                    break;
                }
                else if(*i == "mdx")
                {
                    WSTRACE("Handled by MDXServer");
                    runMdxServer(connectionInfo, "mdx");
                    break;
                }
                WSTRACE("Unhandled");
            }
        }
    }

    WSTRACE("end handleWebSocket");
    return true;
}

void MdxMongooseNFLX::sendMongooseResponse(std::string response, void * conndata)
{
    struct nflx_mg_connection *conn = (struct nflx_mg_connection *)conndata;
    const JSONFormatter f = JSONFormatter();
    const char* mimeType = f.mimeType();
    MdxMongooseNFLX::writeDataF(conn,
                                "HTTP/1.1 200 OK\r\n"
                                "Access-Control-Allow-Origin: *\r\n"
                                "Cache: no-cache\r\n"
                                "Content-Type: %s\r\n"
                                "Connection: close\r\n"
                                "Content-Length: %d\r\n"
                                "Date: %s\r\n"
                                "\r\n"
                                "%s", mimeType, response.size(),
                                Time::now().calendarTimeText().c_str(), response.c_str());
}

int MdxMongooseNFLX::mdxMongooseBeginRequest(struct nflx_mg_connection *conn)
{
    nflx_mg_request_info *requestInfo = nflx_mg_get_request_info(conn);
    bool webSocket = false;

#ifdef BUILD_DEBUG
    NTRACE(TRACE_MDX_MONGOOSE_REQUEST, "Got MDX mongoose request uri: %s method: %s query: %s",
           requestInfo->uri, requestInfo->request_method, requestInfo->query_string);
    for (int i = 0; i < requestInfo->num_headers; ++i)
    {
        NTRACE(TRACE_MDX_MONGOOSE_REQUEST, "header: %d/%d: %s: %s", i, requestInfo->num_headers,
               requestInfo->http_headers[i].name, requestInfo->http_headers[i].value);
    }
#endif

    TIMER(__FUNCTION__, "Mongoose");

    //Send the continue if the client expects one
    const char* expectString = nflx_mg_get_header(conn, "Expect");
    if (expectString && !strcmp(expectString, "100-continue"))
    {
        static const char* ok_continue = "HTTP/1.1 100 Continue\r\n"
            "\r\n";
        static const int okContinueLength = strlen(ok_continue);
        MdxMongooseNFLX::writeData(conn, ok_continue, okContinueLength);
    }

    if (!strcmp(requestInfo->uri, "/ping"))
    {
        handlePing(conn);
        return 1;
    }

    if (!strcmp(requestInfo->uri, "/favicon.ico"))
    {
        MdxMongooseNFLX::writeDataF(conn,
                                    "HTTP/1.1 301 Moved Temporarily\r\n"
                                    "Date: %s\r\n"
                                    "Location: http://www.netflix.com/favicon.ico\r\n"
                                    "\r\n", Time::now().calendarTimeText().c_str());
        return 1;
    }

    if (mWebSocketThread)
    {
        for(int i = 0; i < requestInfo->num_headers; i++)
        {
            if(!strcasecmp(requestInfo->http_headers[i].name, "upgrade") &&
               !strcasecmp(requestInfo->http_headers[i].value, "websocket")) {
                webSocket = true;
                break;
            }
        }

        if(webSocket)
        {
            handleWebSocket(conn, requestInfo);
            return 1;
        }
    }

#ifdef MDXLIB_HAVE_TRACING
    if(!strcmp(requestInfo->uri, "/trace"))
    {
        if(!strcmp(requestInfo->request_method, "GET"))
        {
            Variant report;
            std::string reportString;
            Variant clientInfo;
            clientInfo["version"]       = "";
            clientInfo["command_line"]  = "";
            report["clientInfo"]        = clientInfo;

            if(getTracing().getReport(report) > 0)
            {
                reportString = report.toJSON();

                writeDataF(conn,
                           "HTTP/1.1 200 OK\r\n"
                           "Access-Control-Allow-Origin: *\r\n"
                           "Cache: no-cache\r\n"
                           "Content-Type: text/json\r\n"
                           "Connection: close\r\n"
                           "Date: %s\r\n"
                           "Content-Length: %d\r\n\r\n"
                           "%s", Time::now().calendarTimeText().c_str(), reportString.length(), reportString.c_str());
            }
            else
            {
                writeDataF(conn,
                           "HTTP/1.1 404 Not Found\r\n"
                           "Access-Control-Allow-Origin: *\r\n"
                           "Cache: no-cache\r\n"
                           "Content-Type: text/json\r\n"
                           "Connection: close\r\n"
                           "Date: %s\r\n"
                           "Content-Length: 0\r\n\r\n",
                           Time::now().calendarTimeText().c_str());
            }
        }
        else if(!strcmp(requestInfo->request_method, "POST"))
        {
            std::string responseString = "/trace";
            getTracing().enable();

            writeDataF(conn,
                       "HTTP/1.1 202 Accepted\r\n"
                       "Access-Control-Allow-Origin: *\r\n"
                       "Cache: no-cache\r\n"
                       "Content-Type: text/json\r\n"
                       "Connection: close\r\n"
                       "Date: %s\r\n"
                       "Content-Length: %d\r\n\r\n"
                       "%s",
                       Time::now().calendarTimeText().c_str(),
                       responseString.length(),
                       responseString.c_str());
        }
        else if(!strcmp(requestInfo->request_method, "DELETE"))
        {
            getTracing().clear();
            getTracing().disable();

            writeDataF(conn,
                       "HTTP/1.1 200 OK\r\n"
                       "Access-Control-Allow-Origin: *\r\n"
                       "Cache: no-cache\r\n"
                       "Content-Type: text/json\r\n"
                       "Connection: close\r\n"
                       "Date: %s\r\n"
                       "Content-Length: 0\r\n\r\n",
                       Time::now().calendarTimeText().c_str());

        }

        return 1;
    }
#endif

    if (handleREST(conn, requestInfo))
    {
        return 1;
    }
    Log::error(TRACE_MDX_MONGOOSE, "Unknown path %s", requestInfo->uri);

    return 0;
}

void MdxMongooseNFLX::mdxMongooseEndRequest(const struct nflx_mg_connection *conn, int reply_status_code)
{
    (void)conn;
    (void)reply_status_code;
}

int MdxMongooseNFLX::mdxMongooseLogMessage(const struct nflx_mg_connection *conn, const char *message)
{
    (void)conn;
    (void)message;
    assert(message);
    NTRACE(TRACE_MDX_MONGOOSE, "MdxMongooseNFLX log message: [%s]", message);

    return 0;
}

void MdxMongooseNFLX::mdxMongooseTraceEventStart(int id, const char *message)
{
    (void)id;
    (void)message;
    EVENT_START(message, (ullong)id, "Mongoose","begin",  NULL);
}

void MdxMongooseNFLX::mdxMongooseTraceEventEnd(int id, const char *message)
{
    (void)id;
    (void)message;
    EVENT_FINISH(message, (ullong)id, "Mongoose", "end", NULL);
}

bool MdxMongooseNFLX::writeDataF(struct nflx_mg_connection *conn, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    const std::string str = StringFormatter::vsformat(format, args);
    va_end(args);
    return writeData(conn, str.c_str(), str.size());
}

bool MdxMongooseNFLX::writeData(struct nflx_mg_connection *conn, const char *data, int length)
{
    assert(conn);
    assert(data);
    assert(length > 0);
    int written = 0;
    while (written < length)
    {
        const int l = nflx_mg_write(conn, data + written, length - written);
        if (l <= 0)
        {
#ifdef BUILD_DEBUG
            const int e = errno;
            NTRACE(TRACE_MDX_MONGOOSE, "Failed to write something here: %s (%d) [%d]", data + written, length - written, e);
#endif
            // if (e != EPIPE)
            //     perror("WriteData problem");

            return false;
        }
        written += l;
    }
    return true;
}
