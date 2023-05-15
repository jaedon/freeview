/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "WebSocket.h"
#include <assert.h>
#include <string.h>
#include <nrdbase/Base64.h>
#include <nrdbase/Random.h>
#include <nrdbase/Version.h>
#include <openssl/sha.h>
#include <errno.h>

DEFINE_TRACEAREA(WEBSOCKET, mdx);

// WebSocket protocol constants
static const ullong MIN_FRAME_SIZE    = 2;
static const ullong MAX_FRAME_SIZE    = MIN_FRAME_SIZE + 8 + 4;

// frame bit masks
#define EXTENSIONS_BITMASK          (0x70)
#define FIN_BITMASK                 (0x80)
#define OPCODE_BITMASK              (0x0F)
#define OPCODE_CONTROLFRAME_BITMASK (0x08)
#define MASKING_BITMASK             (0x80)
#define PAYLOADSIZE_BITMASK         (0x7F)

// op code values
#define NO_OP                       (0x0)
#define TEXT_OP                     (0x1)
#define BINARY_OP                   (0x2)
#define CLOSE_OP                    (0x8)
#define PING_OP                     (0x9)
#define PONG_OP                     (0xA)

#define CLOSE_NORMAL                (1000)
#define CLOSE_GOING_AWAY            (1001)
#define CLOSE_PROTOCOL_ERROR        (1002)
#define CLOSE_UNHANDLED_FORMAT      (1003)
#define CLOSE_RESERVED              (1004)
#define CLOSE_RESERVED2             (1005)
#define CLOSE_RESERVED3             (1006)
#define CLOSE_BAD_DATA              (1007)
#define CLOSE_POLICY_ERROR          (1008)
#define CLOSE_TOO_LARGE             (1009)
#define CLOSE_UNHANDLED_EXTENSION   (1010)
#define CLOSE_CANNOT_FULFILL        (1011)
#define CLOSE_RESERVED4             (1015)

// HTTP Constants
static const char *CONNECTION_HEADER           = "Connection";
static const char *UPGRADE_VALUE               = "Upgrade";
static const char *UPGRADE_HEADER              = "Upgrade";
static const char *HOST_HEADER                 = "Host";
static const char *ORIGIN_HEADER               = "Origin";
static const char *WEBSOCKET_VALUE             = "websocket";
static const char *WEBSOCKET_VERSION_HEADER    = "Sec-WebSocket-Version";
static const char *WEBSOCKET_VERSION           = "13";
static const char *WEBSOCKET_PROTOCOL_HEADER   = "Sec-WebSocket-Protocol";
static const char *WEBSOCKET_KEY_HEADER        = "Sec-WebSocket-Key";
static const char *WEBSOCKET_SERVER_KEY        = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
static const char *WEBSOCKET_ACCEPT_HEADER     = "Sec-WebSocket-Accept";

using namespace netflix;
using namespace websocket;

enum FrameType
{
    kError,
    kText,
    kBinary,
    kContinue,
    kPing,
    kPong,
    kControl,   // some other control frame we don't support
    kClose
};

struct FrameDesc
{
    void reset()
    {
        recvd           = 0;
        payloadSize     = 0;
        payloadRecvd    = 0;
        frameSize       = MIN_FRAME_SIZE;
        type            = kError;
        lastFrame       = false;
        masked          = false;
    }

    // frame meta information
    ullong    recvd;

    // frame information
    ullong    payloadSize;
    ullong    payloadRecvd;
    ullong    frameSize;
    FrameType   type;
    uint8_t     mask[4];
    bool        lastFrame;
    bool        masked;
};

// Defaults
const ullong RECV_BUFFER_SIZE = 1 * 1024 * 1024;  // limits the size accumulated before flushing to the callback
const ullong RECV_LIMIT = 1024;                   // limits the size of an individual recv() call
const ullong SEND_LIMIT = 1024;                   // limits the size of an individual send() call

IWebSocketHandler::IWebSocketHandler()
{
}

IWebSocketHandler::~IWebSocketHandler()
{
}

IConnection::IConnection(websocket::socket sock,
                         const std::string &url,
                         const std::string &host,
                         const std::string &client,
                         const std::string &subprotocol,
                         IWebSocketHandler *pHandler)
    : mSocket(sock)
    , mHost(host)
    , mClient(client)
    , mUrl(url)
    , mSubprotocol(subprotocol)
    , mpHandler(pHandler)
{
    WSVERBOSE("IConnetion::IConnection()");
}

IConnection::~IConnection()
{
    WSVERBOSE("IConnection::~IConnection()");
}

std::string IConnection::getHost() const
{
    return mHost;
}

std::string IConnection::getClient() const
{
    return mClient;
}

std::string IConnection::getUrl() const
{
    return mUrl;
}

std::string IConnection::getSubprotocol() const
{
    return mSubprotocol;
}

bool IConnection::closeSocket()
{
    if (mSocket != -1) {
        ::close(mSocket);
        mSocket = -1;
        return true;
    }
    return false;
}

class Connection : public IConnection
{
public:

    enum State
    {
        kClosed,
        kOpen,
        kClosing
    };

    enum IOState
    {
        kIdle,
        kReadingText,
        kReadingBinary,
        kWritingText,
        kWritingBinary
    };

    enum FrameState
    {
        kBegin,
        kFrameBegin,
        kFrameRecvd,
        kPayloadBegin,
        kPayloadRecv,
        kPayloadRecvd
    };

    Connection(bool isClient,
               websocket::socket sock,
               const std::string &url,
               const std::string &host,
               const std::string &client,
               const std::string &subprotocol,
               IWebSocketHandler *pHandler);
    ~Connection();

    ResultType create(const ConnectionConfig &config);

    ResultType sendTextMessage(const void *pData, ullong length);
    ResultType sendBinaryMessage(const void *pData, ullong length);
    ResultType sendMessage(MessageType type, const void *pData, ullong length);
    ResultType beginMessage(MessageType type, const void *pData, ullong length);
    ResultType continueMessage(const void *pData, ullong length);
    ResultType endMessage(const void *pData, ullong length);

    ResultType process();

    bool isOpen() const
    {
        return mState == kOpen;
    }

    bool isClosed() const
    {
        return mState == kClosed;
    }

    ResultType closeConnection();

private:

    bool isReading() const
    {
        return mReadState == kReadingText || mReadState == kReadingBinary;
    }

    bool isWriting() const
    {
        return mWriteState == kWritingText || mWriteState == kWritingBinary;
    }

    websocket::socket getSocket() const
    {
        return mSocket;
    }

    void endAndClose(uint16_t closeCode, const char *pReason = NULL);
    void close(uint16_t closeCode, const char *pReason = NULL);

    bool handlePayload();
    bool checkAndFlushPayload();
    void endConnection(uint16_t closeCode, const char *pReason = NULL);

    // Connection State
    State               mState;

    // Current Frame State
    FrameState          mFrameState;
    FrameDesc           mFrame;

    // Configuration Values
    ullong            mRecvBufferSize;
    ullong            mSendBufferSize;
    ullong            mRecvLimit;
    ullong            mSendLimit;

    // IO States
    IOState             mReadState;
    IOState             mWriteState;
    uint8_t *           mRecvBuffer;
    uint8_t *           mSendBuffer;
    uint32_t            mRecvUtf8State;
    int                 mRecvUtf8Result;
    uint32_t            mSendUtf8State;
    int                 mSendUtf8Result;
    uint16_t            mClosingCode;
    std::string         mClosingReason;
    bool                mIsClient;
};

// Internal function declarations
static std::string  calculateAcceptKey(const std::string &key);

static bool         writeDataF(websocket::socket sock, const char *pFormat, ...);

static ullong     interpretFrameSize(const uint8_t *pInput);
static bool         isControlOp(uint8_t op);
static bool         isValidCloseCode(uint16_t closeCode);
static void         interpretFrame(uint8_t *pInput, ullong inputLength, FrameDesc &frameDesc);
static ResultType   recvFrame(websocket::socket sock, uint8_t *pInputBuffer, FrameDesc &frameDesc);

static ResultType   recvPayload(websocket::socket sock, bool masked, const uint8_t mask[4], void *pData, ullong recvSize, ullong bytesSoFar, ullong &bytesRecvd);
static void         generateMask(uint8_t *pMask);
static ResultType   sendFrame(websocket::socket sock,
                              bool lastFrame,
                              uint8_t op,
                              const void *pData,
                              const uint8_t *pMask,
                              uint8_t *pSendBuffer,
                              ullong payloadLength,
                              ullong sendLimit);
static ResultType   sendCloseFrame(websocket::socket sock, uint16_t closeCode, const void *pData, const uint8_t *pMask, uint8_t *pSendBuffer, ullong payloadLength, ullong sendLimit);
//static void         sendPingFrame(websocket::socket sock, const void *pData, const uint8_t *pMask, uint8_t *pSendBuffer, ullong payloadLength, ullong sendLimit);
static void         sendPongFrame(websocket::socket sock, const void *pData, const uint8_t *pMask, uint8_t *pSendBuffer, ullong payloadLength, ullong sendLimit);

static const char * getErrorString(char *buf, int maxLen, int error);

static bool         isValidUTF8(uint32_t state);
static int          startUTF8(uint8_t c, uint32_t &state);
static int          continueUTF8(uint8_t c, int expected, uint32_t &state);
static int          validateUTF8(uint8_t *pData, ullong length, int expected, uint32_t &state);

//--
// Begin API functions

ResultType websocket::buildClientHandshake(const std::vector<uint8_t> &key,
                                           const std::string &subprotocols,
                                           HttpHeaders &headers)
{
    if(key.size() != 16)
    {
        return InvalidParameter;
    }

    std::vector<uint8_t> base64Key      = Base64::encode(key);
    std::string stringKey(base64Key.begin(), base64Key.end());

    headers[UPGRADE_HEADER]             = WEBSOCKET_VALUE;
    headers[CONNECTION_HEADER]          = UPGRADE_VALUE;
    headers[WEBSOCKET_VERSION_HEADER]   = WEBSOCKET_VERSION;
    headers[WEBSOCKET_KEY_HEADER]       = stringKey;
    if(subprotocols.length())
    {
        headers[WEBSOCKET_PROTOCOL_HEADER]   = subprotocols;
    }

    HttpHeaders::const_iterator header = headers.begin();
    for(; header != headers.end(); ++header)
    {
        WSVERBOSE("%s: %s", header->first.c_str(), header->second.c_str());
    }

    return Success;
}

ResultType websocket::handshakeResponse(socket sock,
                                        const std::string &url,
                                        const std::string &subprotocols,
                                        const std::vector<uint8_t> &key,
                                        const HttpResponseHeaders &headers,
                                        const std::string &localHost,
                                        const std::string &remoteHost,
                                        const ConnectionConfig &config,
                                        IWebSocketHandler *pHandler)
{
    if(!pHandler)
    {
        return InvalidParameter;
    }

    // Validate the response headers
    bool upgrade    = false;
    bool connection = false;
    bool protocol   = true; // implicitly, if no Sec-WebSocket-Protocol header, it is accepting the one subprotocol you specified
    bool accept     = false;
    std::string acceptedProtocol;
    std::vector<uint8_t> base64Key = Base64::encode(key);
    std::string stringKey(base64Key.begin(), base64Key.end());

    std::string expectedKey = calculateAcceptKey(stringKey);

    HttpResponseHeaders::const_iterator header = headers.begin();
    for(;header != headers.end(); ++header)
    {
        WSTRACE("%s: %s", header->header.c_str(), header->value.c_str());

        if(!strcasecmp(header->header.c_str(), UPGRADE_HEADER))
        {
            if(!strcasecmp(header->value.c_str(), WEBSOCKET_VALUE))
            {
                upgrade = true;
            }
        }
        else if(!strcasecmp(header->header.c_str(), CONNECTION_HEADER))
        {
            if(!strcasecmp(header->value.c_str(), UPGRADE_VALUE))
            {
                connection = true;
            }
        }
        else if(!strcasecmp(header->header.c_str(), WEBSOCKET_PROTOCOL_HEADER))
        {
            if(subprotocols.find(header->value) != std::string::npos)
            {
                acceptedProtocol = header->value;
                protocol = true;
            }
            else
            {
                WSWARN("Response subprotocol %s not in requested list %s", header->value.c_str(), subprotocols.c_str());
                protocol = false;
            }
        }
        else if(!strcasecmp(header->header.c_str(), WEBSOCKET_ACCEPT_HEADER))
        {
            if(expectedKey == header->value)
            {
                accept = true;
            }
        }
    }

    uint8_t sendBuffer[MAX_FRAME_SIZE];

    if(!upgrade || !connection || !accept || !protocol)
    {
        if(!upgrade)    { WSWARN("Wrong or missing %s header", UPGRADE_HEADER); }
        if(!connection) { WSWARN("Wrong or missing %s header", CONNECTION_HEADER); }
        if(!accept)     { WSWARN("Wrong or missing %s header", WEBSOCKET_ACCEPT_HEADER); }
        if(!protocol)   { WSWARN("Wrong or missing %s header", WEBSOCKET_PROTOCOL_HEADER); }

        sendCloseFrame(sock, CLOSE_PROTOCOL_ERROR, NULL, NULL, sendBuffer, 0, SEND_LIMIT);

        return ProtocolError;
    }

    Connection *pConnection = new Connection(true,
                                             sock,
                                             url,
                                             remoteHost,
                                             localHost,
                                             acceptedProtocol,
                                             pHandler);

    if(!pConnection)
    {
        sendCloseFrame(sock, CLOSE_CANNOT_FULFILL, NULL, NULL, sendBuffer, 0, SEND_LIMIT);

        WSERROR("Out of memory");
        return NoMemory;
    }

    ResultType result = pConnection->create(config);

    if(Success != result)
    {
        delete pConnection;

        sendCloseFrame(sock, CLOSE_CANNOT_FULFILL, NULL, NULL, sendBuffer, 0, SEND_LIMIT);

        return result;
    }

    pHandler->mpConnection.reset(pConnection);

    return Success;
}

ResultType websocket::handshake(socket sock,
                                const std::string &url,
                                const HttpHeaders &requestHeaders,
                                const std::string &client,
                                ConnectionInfo &connectionInfo)
{
    connectionInfo.mSocket = sock;
    connectionInfo.mUrl = url;
    connectionInfo.mClient = client;

    bool connection = false;
    bool upgrade = false;
    bool key = false;
    bool protocol = true; // by default an absense of protocol is not an error
    bool version = false;

    HttpHeaders::const_iterator header = requestHeaders.begin();
    for(; header != requestHeaders.end(); ++header)
    {
        const std::string &headerName = header->first;
        const std::string &headerValue = header->second;

        if(!strcasecmp(headerName.c_str(), CONNECTION_HEADER))
        {
            if(!strcasecmp(headerValue.c_str(), UPGRADE_VALUE))
            {
                connection = true;
            }
        }
        else if(!strcasecmp(headerName.c_str(), UPGRADE_HEADER))
        {
            if(!strcasecmp(headerValue.c_str(), WEBSOCKET_VALUE))
            {
                upgrade = true;
            }
        }
        else if(!strcasecmp(headerName.c_str(), HOST_HEADER))
        {
            connectionInfo.mHost = headerValue;
        }
        else if(!strcasecmp(headerName.c_str(), ORIGIN_HEADER))
        {
            connectionInfo.mOrigin = headerValue;
        }
        else if(!strcasecmp(headerName.c_str(), WEBSOCKET_VERSION_HEADER))
        {
            if(!strcasecmp(headerValue.c_str(), WEBSOCKET_VERSION))
            {
                version = true;
            }
        }
        else if(!strcasecmp(headerName.c_str(), WEBSOCKET_KEY_HEADER))
        {
            connectionInfo.mRequestKey = headerValue;
            key = true;
        }
        else if(!strcasecmp(headerName.c_str(), WEBSOCKET_PROTOCOL_HEADER))
        {
            // There can be multiple subprotocol header lines

            // and each subprotocol header line can have multiple subprotocols
            std::stringstream subprotocols(headerValue);
            std::string subprotocol;

            // extract one subprotocol from this header
            subprotocols >> subprotocol;

            while(!subprotocol.empty())
            {
                connectionInfo.mSubprotocols.push_back(subprotocol);

                // extract next subprotocol from this line
                subprotocol = "";
                subprotocols >> subprotocol;
            }
        }
    }

    if(!connection || !upgrade || !key || !protocol || !version)
    {
        if(!connection) { WSWARN("Missing or invalid %s", CONNECTION_HEADER); }
        if(!upgrade) { WSWARN("Missing or invalid %s", UPGRADE_HEADER); }
        if(!key) { WSWARN("Missing or invalid %s", WEBSOCKET_KEY_HEADER); }
        if(!protocol) { WSWARN("Missing or invalid %s", WEBSOCKET_PROTOCOL_HEADER); }
        if(!version) { WSWARN("Missing or invalid %s", WEBSOCKET_VERSION_HEADER); }

        writeDataF(sock, "HTTP/1.1 400 Bad Request\r\n"
                   "%s: %s\r\n"
                   "\r\n",
                   WEBSOCKET_VERSION_HEADER,
                   WEBSOCKET_VERSION);

        return ProtocolError;
    }

    return Success;
}

void websocket::defaultConfig(websocket::ConnectionConfig &config)
{
    config.mRecvBufferSize = RECV_BUFFER_SIZE;
    config.mRecvLimit = RECV_LIMIT;
    config.mSendLimit = SEND_LIMIT;
}

ResultType websocket::acceptConnection(const ConnectionInfo &connectionInfo,
                                       const std::string &subprotocol,
                                       const ConnectionConfig &config,
                                       IWebSocketHandler *pHandler)
{
    if(!pHandler)
    {
        return InvalidParameter;
    }

    Connection *pConnection = new Connection(false,
                                             connectionInfo.mSocket,
                                             connectionInfo.mUrl,
                                             connectionInfo.mHost,
                                             connectionInfo.mClient,
                                             subprotocol,
                                             pHandler);

    if(!pConnection)
    {
        writeDataF(connectionInfo.mSocket, "HTTP/1.1 500 Internal Server Error\r\n"
                   "\r\n");

        WSERROR("Out of memory");
        return NoMemory;
    }

    ResultType protocolResult = pConnection->create(config);

    if(Success != protocolResult)
    {
        delete pConnection;

        writeDataF(connectionInfo.mSocket, "HTTP/1.1 500 Internal Server Error\r\n"
                   "\r\n");

        return protocolResult;
    }

    pHandler->mpConnection.reset(pConnection);

    std::string responseKey;
    responseKey = calculateAcceptKey(connectionInfo.mRequestKey);

    WSTRACE("Switching protocols to websocket: %s %s", responseKey.c_str(), subprotocol.c_str());
    if(writeDataF(connectionInfo.mSocket, "HTTP/1.1 101 Switching Protocols\r\n"
                  "%s: %s\r\n"
                  "%s: %s\r\n"
                  "%s: %s\r\n"
                  "%s: %s\r\n"
                  "\r\n",
                  UPGRADE_HEADER,
                  WEBSOCKET_VALUE,
                  CONNECTION_HEADER,
                  UPGRADE_VALUE,
                  WEBSOCKET_ACCEPT_HEADER,
                  responseKey.c_str(),
                  WEBSOCKET_PROTOCOL_HEADER,
                  subprotocol.c_str()))
    {
        WSTRACE("Websocket connected");
    }
    else
    {
        return NetworkError;
    }

    return Success;
}

ResultType websocket::rejectConnection(const websocket::ConnectionInfo &connectionInfo)
{
    writeDataF(connectionInfo.mSocket, "HTTP/1.1 403 Forbidden\r\n");

    return Success;
}

// End API Functions
//--

Connection::Connection(bool isClient,
                       websocket::socket sock,
                       const std::string &url,
                       const std::string &host,
                       const std::string &client,
                       const std::string &subprotocol,
                       IWebSocketHandler *pHandler)
    : IConnection(sock, url, host, client, subprotocol, pHandler)
    , mState(kClosed)
    , mFrameState(kBegin)
    , mReadState(kIdle)
    , mWriteState(kIdle)
    , mRecvBuffer(NULL)
    , mSendBuffer(NULL)
    , mRecvUtf8State(0)
    , mRecvUtf8Result(0)
    , mSendUtf8State(0)
    , mSendUtf8Result(0)
    , mIsClient(isClient)
{
}

Connection::~Connection()
{
    delete[] mRecvBuffer;
    delete[] mSendBuffer;
}

ResultType Connection::process()
{
    ResultType result = Success;
    ullong bytesRecvd;
    uint16_t closeCode;
    const char *pCloseReason = NULL;
    uint8_t mask[4];

    if(mState == Connection::kClosed) {
        return InvalidCall;
    }

    do
    {
        switch(mFrameState)
        {
        case kBegin:
            mFrame.reset();
            mFrameState = kFrameBegin;
            // fallthrough
        case kFrameBegin:

            result = recvFrame(mSocket, mRecvBuffer, mFrame);
            if(result != Success)
            {
                if(result == Continue)
                {
                    return result;
                }
                else if(result != NetworkError)
                {
                    endAndClose(CLOSE_PROTOCOL_ERROR);
                    return ProtocolError;
                }
                else
                {
                    this->close(CLOSE_GOING_AWAY);
                    return NetworkError;
                }
            }

            if(mFrame.recvd == mFrame.frameSize)
            {
                interpretFrame(mRecvBuffer, mFrame.recvd, mFrame);

                if(mFrame.type == kError)
                {
                    endAndClose(CLOSE_PROTOCOL_ERROR);
                    return ProtocolError;
                }
                else
                {
                    mFrame.recvd = 0;
                    mFrameState = kFrameRecvd;
                }
            }
            assert(mFrame.recvd < MIN_FRAME_SIZE || mFrame.recvd <= mFrame.frameSize);
            break;

        case kFrameRecvd:
            // have a whole frame description
            switch(mFrame.type)
            {
            case kContinue:
                if(!isReading())
                {
                    WSERROR("Expected opening frame but got continue");
                    endAndClose(CLOSE_PROTOCOL_ERROR);
                    return ProtocolError;
                }
                else
                {
                    WSVERBOSE("Continuing fragmented message: %lld bytes\n", mFrame.payloadSize);
                    mFrameState = kPayloadBegin;
                }
                break;

            case kText:
                if(isReading())
                {
                    WSERROR("Expected continuation of fragmented message");
                    endAndClose(CLOSE_PROTOCOL_ERROR);
                    return ProtocolError;
                }
                else
                {
                    WSVERBOSE("Read text frame claiming %lld payload bytes", mFrame.payloadSize);
                    mReadState = Connection::kReadingText;
                    mpHandler->onMessageBegin(TextMessage);
                    mFrameState = kPayloadBegin;
                }
                break;

            case kBinary:
                if(mReadState != Connection::kIdle)
                {
                    WSERROR("Expected continuation of fragmented message");
                    endAndClose(CLOSE_PROTOCOL_ERROR);
                    return ProtocolError;
                }
                else
                {
                    WSVERBOSE("Read binary frame claiming %lld payload bytes", mFrame.payloadSize);
                    mReadState = Connection::kReadingBinary;
                    mpHandler->onMessageBegin(BinaryMessage);
                    mFrameState = kPayloadBegin;
                }
                break;

            case kPing:
                WSVERBOSE("Ping frame received claiming %lld bytes", mFrame.payloadSize);
                if(mFrame.payloadSize > 125)
                {
                    endAndClose(CLOSE_PROTOCOL_ERROR);
                    return ProtocolError;
                }
                else
                {
                    mFrameState = kPayloadBegin;
                }
                break;

            case kPong:
                WSVERBOSE("Pong frame received claiming %lld bytes", mFrame.payloadSize);
                if(mFrame.payloadSize > 125)
                {
                    endAndClose(CLOSE_PROTOCOL_ERROR);
                    return ProtocolError;
                }
                else
                {
                    mFrameState = kPayloadBegin;
                }
                break;

            case kClose:
                WSVERBOSE("Read close frame claiming %lld payload bytes", mFrame.payloadSize);
                if(mState == Connection::kClosing)
                {
                    WSVERBOSE("Finished close initiated locally");
                    this->close(this->mClosingCode, this->mClosingReason.c_str());
                    return Success;
                }
                else
                {
                    WSVERBOSE("Closing initiated remotely");
                    if(mFrame.payloadSize == 0)
                    {
                        // Valid empty close, return normal closure code
                        WSVERBOSE("Close frame with no payload. Finished close initiated remotely");

                        endAndClose(CLOSE_NORMAL);
                        return Success;
                    }
                    else if(mFrame.payloadSize > 0 && mFrame.payloadSize < 2)
                    {
                        WSERROR("Close frame %lld bytes payload too small", mFrame.payloadSize);
                        endAndClose(CLOSE_PROTOCOL_ERROR);
                        return ProtocolError;
                    }
                    else if(mFrame.payloadSize >= 2 && mFrame.payloadSize <= 125)
                    {
                        mFrameState = kPayloadBegin;
                    }
                    else
                    {
                        WSERROR("Close frame %lld bytes payload too large", mFrame.payloadSize);
                        endAndClose(CLOSE_PROTOCOL_ERROR);
                        return ProtocolError;
                    }
                }
                break;

            case kControl:
                WSVERBOSE("Other Control frame received");
                endAndClose(CLOSE_PROTOCOL_ERROR);
                return ProtocolError;

            case kError:
                WSVERBOSE("Error in frame received");
                endAndClose(CLOSE_PROTOCOL_ERROR);
                return ProtocolError;

            default:
                // some kind of internal state error
                break;
            }
            break;

        case kPayloadBegin:
            mFrame.recvd        = 0;
            mFrame.payloadRecvd = 0;
            mFrameState         = kPayloadRecv;
            // fall through
        case kPayloadRecv:
            switch(mFrame.type)
            {
            case kContinue:
            case kText:
            case kBinary:
                if(!handlePayload())
                {
                    endAndClose(CLOSE_BAD_DATA);
                    return ProtocolError;
                }
                if(mFrame.payloadRecvd == mFrame.payloadSize)
                {
                    mFrameState = kPayloadRecvd;
                }

                assert(mFrame.payloadRecvd <= mFrame.payloadSize);
                break;

            case kPing:
                result = recvPayload(mSocket,
                                     mFrame.masked,
                                     mFrame.mask,
                                     mRecvBuffer + mFrame.recvd,
                                     mFrame.payloadSize - mFrame.payloadRecvd,
                                     mFrame.payloadRecvd,
                                     bytesRecvd);

                if(result != Success && result != Continue)
                {
                    if(result != NetworkError)
                    {
                        endAndClose(CLOSE_BAD_DATA);
                        return result;
                    }
                    else
                    {
                        this->close(CLOSE_GOING_AWAY);
                        return result;
                    }
                }

                mFrame.recvd += bytesRecvd;
                mFrame.payloadRecvd += bytesRecvd;

                WSVERBOSE("Ping frame %lld bytes payload read", mFrame.payloadRecvd);

                if(mFrame.payloadRecvd == mFrame.payloadSize)
                {
                    mFrameState = kPayloadRecvd;
                }

                assert(mFrame.payloadRecvd <= mFrame.payloadSize);
                break;

            case kPong:
                result = recvPayload(mSocket,
                                     mFrame.masked,
                                     mFrame.mask,
                                     mRecvBuffer + mFrame.recvd,
                                     mFrame.payloadSize - mFrame.payloadRecvd,
                                     mFrame.payloadRecvd,
                                     bytesRecvd);

                if(result != Success && result != Continue)
                {
                    if(result != NetworkError)
                    {
                        endAndClose(CLOSE_BAD_DATA);
                        return NetworkError;
                    }
                    else
                    {
                        this->close(CLOSE_GOING_AWAY);
                        return result;
                    }
                }

                mFrame.recvd += bytesRecvd;
                mFrame.payloadRecvd += bytesRecvd;

                WSVERBOSE("Pong frame %lld bytes payload read", mFrame.payloadRecvd);

                if(mFrame.payloadRecvd == mFrame.payloadSize)
                {
                    mFrameState = kPayloadRecvd;
                }

                assert(mFrame.payloadRecvd <= mFrame.payloadSize);
                break;

            case kClose:
                // Valid sized payload, return their closeCode and pCloseReason to them
                result = recvPayload(mSocket,
                                     mFrame.masked,
                                     mFrame.mask,
                                     mRecvBuffer + mFrame.recvd,
                                     mFrame.payloadSize - mFrame.payloadRecvd,
                                     mFrame.payloadRecvd,
                                     bytesRecvd);

                if(result != Success && result != Continue)
                {
                    if(result != NetworkError)
                    {
                        endAndClose(CLOSE_BAD_DATA);
                        return result;
                    }
                    else
                    {
                        this->close(CLOSE_GOING_AWAY);
                        return NetworkError;
                    }
                }

                mFrame.recvd += bytesRecvd;
                mFrame.payloadRecvd += bytesRecvd;

                WSVERBOSE("Close frame %lld bytes payload read", mFrame.payloadRecvd);

                if(mFrame.payloadRecvd == mFrame.payloadSize)
                {
                    mFrameState = kPayloadRecvd;
                }

                assert(mFrame.payloadRecvd <= mFrame.payloadSize);
                break;

            case kControl:
            case kError:
                // how did this happen?
            default:
                // some kind of internal state error
                break;
            }
            break;

        case kPayloadRecvd:
            switch(mFrame.type)
            {
            case kContinue:
            case kText:
            case kBinary:
                if(!checkAndFlushPayload())
                {
                    endAndClose(CLOSE_BAD_DATA);
                    return ProtocolError;
                }
                WSVERBOSE("Data frame finished");
                if(mFrame.lastFrame)
                {
                    mReadState = Connection::kIdle;
                    mpHandler->onMessageEnd();
                }
                mFrameState = kBegin;
                break;

            case kPing:
                WSVERBOSE("Ping frame finished");
                if(mIsClient)
                {
                    generateMask(mask);
                }
                sendPongFrame(mSocket, mRecvBuffer, mIsClient ? mask : NULL, mSendBuffer, mFrame.payloadSize, mSendLimit);
                mFrameState = kBegin;
                break;

            case kPong:
                WSVERBOSE("Pong frame finished");
                mFrameState = kBegin;
                break;

            case kClose:
                WSVERBOSE("Finished close initiated remotely");
                closeCode = ((uint16_t)mRecvBuffer[0] << 8) | ((uint16_t)mRecvBuffer[1]);
                if(mFrame.payloadSize > 2)
                {
                    int result = validateUTF8(mRecvBuffer + 2,
                                              mFrame.payloadSize - 2,
                                              0,
                                              mRecvUtf8State);
                    if(result != 0)
                    {
                        WSERROR("Invalid UTF-8 detected in close frame");
                        closeCode = CLOSE_BAD_DATA;
                        pCloseReason = "Bad Data";
                    }
                    else
                    {
                        pCloseReason = (const char *)&mRecvBuffer[2];
                        mRecvBuffer[mFrame.payloadSize] = '\0';
                    }
                }
                else
                {
                    pCloseReason = NULL;
                }

                WSVERBOSE("Close frame %lld bytes payload. Close Code=%d.", mFrame.payloadRecvd, closeCode);

                if(!isValidCloseCode(closeCode))
                {
                    closeCode = CLOSE_PROTOCOL_ERROR;
                    WSERROR("Close code is invalid, returning close code %d", closeCode);
                }
                endAndClose(closeCode, pCloseReason);
                return ProtocolError;

            case kControl:
            case kError:
                // how did this happen?
            default:
                // some kind of internal state error
                break;
            }
            break;

        default:
            break;
        }
    } while(mState != Connection::kClosed && (mFrameState == kFrameRecvd || mFrameState == kPayloadBegin || mFrameState == kPayloadRecvd));

    return result;
}

ResultType Connection::closeConnection()
{
    if(isOpen())
    {
        endConnection(CLOSE_NORMAL);
    }

    return Success;
}

ResultType Connection::create(const ConnectionConfig &config)
{
    uint8_t *pRecvBuffer = new uint8_t[config.mRecvBufferSize];
    if(!pRecvBuffer)
    {
        WSWARN("Could not allocate receive buffer %lld", config.mRecvBufferSize);
        return NoMemory;
    }

    uint8_t *pSendBuffer = new uint8_t[config.mSendLimit + MAX_FRAME_SIZE];
    if(!pSendBuffer)
    {
        delete[] pRecvBuffer;

        WSWARN("Could not allocate send buffer %lld", config.mSendLimit + MAX_FRAME_SIZE);
        return NoMemory;
    }

    mState          = kOpen;
    mRecvBuffer     = pRecvBuffer;
    mSendBuffer     = pSendBuffer;
    mRecvUtf8State  = 0;
    mRecvUtf8Result = 0;

    mRecvBufferSize = config.mRecvBufferSize;
    mSendBufferSize = config.mSendLimit + MAX_FRAME_SIZE;
    mRecvLimit      = config.mRecvLimit;
    mSendLimit      = mSendBufferSize;//config.mSendLimit;

    return Success;
}

ResultType Connection::sendTextMessage(const void *pData, ullong length)
{
    return sendMessage(TextMessage, pData, length);
}

ResultType Connection::sendBinaryMessage(const void *pData, ullong length)
{
    return sendMessage(BinaryMessage, pData, length);
}

ResultType Connection::sendMessage(MessageType type, const void *pData, ullong length)
{
    if(!pData && length)
    {
        return InvalidParameter;
    }

    uint8_t mask[4];

    if(mIsClient)
    {
        generateMask(mask);
    }

    ResultType result;
    result = sendFrame(mSocket,
                       true,
                       type == TextMessage ? TEXT_OP : BINARY_OP,
                       pData,
                       mIsClient ? mask : NULL,
                       mSendBuffer,
                       length,
                       mSendLimit);


    if(result != Success)
    {
        if(result == Continue)
        {
        }
        else if(result != NetworkError)
        {
            endAndClose(CLOSE_PROTOCOL_ERROR);
        }
        else
        {
            this->close(CLOSE_GOING_AWAY);
        }
    }

    return result;
}

ResultType Connection::beginMessage(MessageType type, const void *pData, ullong length)
{
    if(!pData && length)
    {
        return InvalidParameter;
    }

    uint8_t op;

    if(type == TextMessage)
    {
        if(pData && length)
        {
            mSendUtf8Result = validateUTF8((uint8_t *)pData,
                                           length,
                                           0,
                                           mSendUtf8State);
            if(mSendUtf8Result < 0)
            {
                WSERROR("Cannot send invalid UTF-8");
                return InvalidData;
            }
        }

        mWriteState = Connection::kWritingText;
        op = TEXT_OP;
    }
    else if(type == BinaryMessage)
    {
        mWriteState = Connection::kWritingBinary;
        op = BINARY_OP;
    }
    else
    {
        return InvalidParameter;
    }

    uint8_t mask[4];

    if(mIsClient)
    {
        generateMask(mask);
    }

    ResultType result;
    result = sendFrame(mSocket,
                       false,
                       op,
                       pData,
                       mIsClient ? mask : NULL,
                       mSendBuffer,
                       length,
                       mSendLimit);

    if(result != Success)
    {
        if(result == Continue)
        {
        }
        else if(result != NetworkError)
        {
            endAndClose(CLOSE_PROTOCOL_ERROR);
        }
        else
        {
            this->close(CLOSE_GOING_AWAY);
        }
    }

    return result;
}

ResultType Connection::continueMessage(const void *pData, ullong length)
{
    if(!isWriting())
    {
        return InvalidCall;
    }

    if(!pData && length)
    {
        return InvalidParameter;
    }

    if(mWriteState == kWritingText)
    {
        if(pData && length)
        {
            mSendUtf8Result = validateUTF8((uint8_t *)pData,
                                           length,
                                           mSendUtf8Result,
                                           mSendUtf8State);
            if(mSendUtf8Result < 0)
            {
                WSERROR("Cannot send invalid UTF-8");
                return InvalidData;
            }
        }
    }

    uint8_t mask[4];

    if(mIsClient)
    {
        generateMask(mask);
    }

    ResultType result;
    result = sendFrame(mSocket,
                       false,
                       NO_OP,
                       pData,
                       mIsClient ? mask : NULL,
                       mSendBuffer,
                       length,
                       mSendLimit);

    if(result != Success)
    {
        if(result == Continue)
        {
        }
        else if(result != NetworkError)
        {
            endAndClose(CLOSE_PROTOCOL_ERROR);
        }
        else
        {
            this->close(CLOSE_GOING_AWAY);
        }
    }

    return result;
}

ResultType Connection::endMessage(const void *pData, ullong length)
{
    if(!isWriting())
    {
        return InvalidCall;
    }

    if(!pData && length)
    {
        return InvalidParameter;
    }

    if(mWriteState == Connection::kWritingText)
    {
        if(pData && length)
        {
            mSendUtf8Result = validateUTF8((uint8_t *)pData,
                                           length,
                                           mSendUtf8Result,
                                           mSendUtf8State);

            if(mSendUtf8Result != 0)
            {
                WSERROR("Cannot end on invalid UTF-8");

                // cannot end a message on an invalid UTF8 code point
                return InvalidData;
            }
        }
    }

    uint8_t mask[4];

    if(mIsClient)
    {
        generateMask(mask);
    }

    ResultType result;
    result = sendFrame(mSocket,
                       true,
                       NO_OP,
                       pData,
                       mIsClient ? mask : NULL,
                       mSendBuffer,
                       length,
                       mSendLimit);

    if(result != Success)
    {
        if(result == Continue)
        {
        }
        else if(result != NetworkError)
        {
            endAndClose(CLOSE_PROTOCOL_ERROR);
        }
        else
        {
            this->close(CLOSE_GOING_AWAY);
        }
    }

    return result;
}

bool Connection::handlePayload()
{
    ullong maxDataLength = mRecvLimit;
    ullong bytesLeft = mFrame.payloadSize - mFrame.payloadRecvd;

    // get the min of space in the buffer or left in the payload
    ullong bytesToRecv = maxDataLength > bytesLeft ? bytesLeft : maxDataLength;

    // if the amount to get would not fit in the space left in the buffer,
    // flush the buffer
    if(mFrame.recvd + bytesToRecv >= mRecvBufferSize)
    {
        // check and flush data to handler
        if(!checkAndFlushPayload())
        {
            return false;
        }
    }

    ullong bytesRecvd = 0;

    ResultType result = recvPayload(getSocket(),
                                    mFrame.masked,
                                    mFrame.mask,
                                    mRecvBuffer + mFrame.recvd,
                                    bytesToRecv,
                                    mFrame.payloadRecvd,
                                    bytesRecvd);

    if(result != Success && result != Continue)
    {
        return false;
    }

    mFrame.recvd += bytesRecvd;
    mFrame.payloadRecvd += bytesRecvd;

    if(bytesRecvd)
    {
        WSVERBOSE("recvd %lld of %lld bytes", mFrame.payloadRecvd, mFrame.payloadSize);
    }

    return true;
}

bool Connection::checkAndFlushPayload()
{
    if(mFrame.recvd)
    {
        // check and flush data to handler
        if(mReadState == kReadingText)
        {
            mRecvUtf8Result = validateUTF8(mRecvBuffer,
                                           mFrame.recvd,
                                           mRecvUtf8Result,
                                           mRecvUtf8State);

            bool isError = false;

            // if it is the end of data
            if(mFrame.lastFrame && (mRecvUtf8Result != 0))
            {
                // error if not end of code point or invalid code point
                isError = true;
            }
            else if(mRecvUtf8Result < 0)
            {
                // error if invalid code point
                isError = true;
            }

            if(isError)
            {
                mpHandler->onError();

                WSERROR("Invalid UTF-8 detected in payload");
                return false;
            }
        }

        WSVERBOSE("Flushing %lld payload bytes", mFrame.recvd);
        mpHandler->onData(mRecvBuffer, mFrame.recvd);

        mFrame.recvd = 0;
    }

    return true;
}

void Connection::endConnection(uint16_t closeCode, const char *pReason)
{
    if(mState == Connection::kOpen)
    {
        mReadState = Connection::kIdle;

        uint8_t mask[4];
        if(mIsClient)
        {
            generateMask(mask);
        }

        ResultType result = sendCloseFrame(getSocket(), closeCode, pReason, mIsClient ? mask : NULL, mSendBuffer, pReason ? strlen(pReason) : 0, mSendLimit);

        if(result == Success)
        {
            mState = Connection::kClosing;
            mClosingCode = closeCode;
            mClosingReason = pReason ? pReason : "";
        }
        else
        {
            mWriteState = Connection::kIdle;
            close(closeCode, pReason);
        }
    }
}

void Connection::endAndClose(uint16_t closeCode, const char *pReason)
{
    endConnection(closeCode, pReason);
    mFrameState = kBegin;
    this->close(closeCode, pReason);
}

void Connection::close(uint16_t closeCode, const char *pReason)
{
    mState = Connection::kClosed;
    mpHandler->onClose(closeCode, pReason ? pReason : "");
}

//--
// Begin private functions
std::string calculateAcceptKey(const std::string &key)
{
    // concatenate the keys
    std::string finalKey = key + WEBSOCKET_SERVER_KEY;
    std::vector<unsigned char> finalKeyData(finalKey.begin(), finalKey.end());
    std::vector<unsigned char> digest(SHA_DIGEST_LENGTH);

    // calculate the sha1 hash of the concatenated keys
    SHA1(&finalKeyData[0], finalKeyData.size(), &digest[0]);

    return Base64::encode(std::string(digest.begin(), digest.end()));
}

bool writeDataF(websocket::socket sock, const char *pFormat, ...)
{
    va_list args;
    va_start(args, pFormat);

    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    char errBuf[512];

    int size = vsnprintf(buffer, BUFFER_SIZE, pFormat, args);

    va_end(args);

    if(size > BUFFER_SIZE)
    {
        WSERROR("Data truncated to %d bytes", BUFFER_SIZE);
        size = BUFFER_SIZE;
    }

    if(size)
    {
        ssize_t bytesSent = 0;

        do
        {
            ssize_t sent = send(sock, buffer, size, 0);

            if(sent < 0)
            {
                WSERROR("Failed to send data. errno = %d %s", errno, getErrorString(errBuf, sizeof(errBuf), errno));

                return false;
            }

            bytesSent += sent;

        } while(bytesSent < size);
    }

    return true;
}

ullong interpretFrameSize(const uint8_t *pInput)
{
    ullong frameSize = 2;

    uint8_t payloadSize7 = pInput[1] & PAYLOADSIZE_BITMASK;

    if(payloadSize7 == 126)
    {
        frameSize += 2;
    }
    else if(payloadSize7 == 127)
    {
        frameSize += 8;
    }

    if((pInput[1] & MASKING_BITMASK) == MASKING_BITMASK)
    {
        frameSize += 4;
    }

    return frameSize;
}

bool isControlOp(uint8_t op)
{
    return ((op & OPCODE_CONTROLFRAME_BITMASK) == OPCODE_CONTROLFRAME_BITMASK);
}

bool isValidCloseCode(uint16_t closeCode)
{
    return ((closeCode == CLOSE_NORMAL) ||
            (closeCode == CLOSE_GOING_AWAY) ||
            (closeCode == CLOSE_PROTOCOL_ERROR) ||
            (closeCode == CLOSE_UNHANDLED_FORMAT) ||
            (closeCode == CLOSE_BAD_DATA) ||
            (closeCode == CLOSE_POLICY_ERROR) ||
            (closeCode == CLOSE_TOO_LARGE) ||
            (closeCode == CLOSE_UNHANDLED_EXTENSION) ||
            (closeCode == CLOSE_CANNOT_FULFILL) ||
            (closeCode >= 3000 && closeCode < 4000) ||
            (closeCode >= 4000 && closeCode < 5000));
}

void interpretFrame(uint8_t *pInput, ullong inputLength, FrameDesc &frameDesc)
{
    WSVERBOSE("Raw frame: %02x %02x", pInput[0], pInput[1]);

    frameDesc.frameSize = 0;

    if(!pInput || inputLength < 2)
    {
        WSERROR("Null input?");
        frameDesc.type = kError;
        frameDesc.lastFrame = true;
        frameDesc.payloadSize = 0;
        return;
    }

    frameDesc.frameSize = 2;

    uint8_t op = pInput[0] & OPCODE_BITMASK;

    switch(op)
    {
    case NO_OP:
        frameDesc.type = kContinue;
        break;

    case TEXT_OP:
        frameDesc.type = kText;
        break;

    case BINARY_OP:
        frameDesc.type = kBinary;
        break;

    case CLOSE_OP:
        frameDesc.type = kClose;
        break;

    case PING_OP:
        frameDesc.type = kPing;
        break;

    case PONG_OP:
        frameDesc.type = kPong;
        break;

    default:
        if(isControlOp(op))
        {
            WSVERBOSE("Control opcode: %02x\n", op);
            frameDesc.type = kControl;
        }
        else
        {
            WSERROR("Unhandled frame opcode: %02x\n", op);
            frameDesc.type = kError;
        }
        break;
    }

    if((pInput[0] & FIN_BITMASK) == FIN_BITMASK)
    {
        frameDesc.lastFrame = true;
    }
    else
    {
        if(isControlOp(op))
        {
            WSERROR("FIN bit not set, OP is a control OP (0x%.2x)", op);
            frameDesc.type = kError;
        }

        frameDesc.lastFrame = false;
    }

    uint8_t payloadSize7 = pInput[1] & PAYLOADSIZE_BITMASK;

    if(payloadSize7 == 126)
    {
        WSVERBOSE("Extended size16: %02x %02x", pInput[2], pInput[3]);

        // 16 bit payload size
        frameDesc.payloadSize = ((uint16_t)pInput[2]) << 8 | (uint16_t)pInput[3];

        frameDesc.frameSize += 2;
    }
    else if(payloadSize7 == 127)
    {
        WSVERBOSE("Extended size64: %02x %02x %02x %02x %02x %02x %02x %02x",
                pInput[2], pInput[3], pInput[4], pInput[5], pInput[6], pInput[7], pInput[8], pInput[9]);

        // 64 bit payload size
        frameDesc.payloadSize = ((((ullong)pInput[2]) << 56) |
                                 (((ullong)pInput[3]) << 48) |
                                 (((ullong)pInput[4]) << 40) |
                                 (((ullong)pInput[5]) << 32) |
                                 (((ullong)pInput[6]) << 24) |
                                 (((ullong)pInput[7]) << 16) |
                                 (((ullong)pInput[8]) << 8) |
                                 (((ullong)pInput[9])));

        frameDesc.frameSize += 8;
    }
    else
    {
        // 7 bit payload size
        frameDesc.payloadSize = (ullong)payloadSize7;
    }

    if((pInput[1] & MASKING_BITMASK) == MASKING_BITMASK)
    {
        frameDesc.masked = true;
        frameDesc.mask[0] = pInput[frameDesc.frameSize];
        frameDesc.mask[1] = pInput[frameDesc.frameSize + 1];
        frameDesc.mask[2] = pInput[frameDesc.frameSize + 2];
        frameDesc.mask[3] = pInput[frameDesc.frameSize + 3];

        frameDesc.frameSize += 4;
    }
    else
    {
        frameDesc.masked = false;
    }

    if((pInput[0] & EXTENSIONS_BITMASK) != 0x0)
    {
        WSERROR("Unsupported extension bits set on incoming WebSocket frame.");
        // we don't support any extensions

        frameDesc.type = kError;
        frameDesc.lastFrame = true;
    }

    WSVERBOSE("Frame: type=%d, size=%lld, fin=%s, masked=%s, payload=%lld",
              frameDesc.type,
              frameDesc.frameSize,
              frameDesc.lastFrame ? "true" : "false",
              frameDesc.masked ? "true" : "false",
              frameDesc.payloadSize);
}

websocket::ResultType recvFrame(websocket::socket sock, uint8_t *inputBuffer, FrameDesc &frameDesc)
{
    ssize_t bytesRead;
    char errBuf[512];

    // we need to receive at least MIN_FRAME_SIZE bytes
    if(frameDesc.recvd < MIN_FRAME_SIZE)
    {
        bytesRead = recv(sock, inputBuffer + frameDesc.recvd, MIN_FRAME_SIZE - frameDesc.recvd, 0);

        switch (bytesRead) {
        case 0:
            return NetworkError;
        case -1:
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                return Continue;
            }

            WSWARN("recvFrame errno = %d %s", errno, getErrorString(errBuf, sizeof(errBuf), errno));

            return NetworkError;
        default:
            break;
        }

        frameDesc.recvd += bytesRead;
    }

    if(frameDesc.recvd < MIN_FRAME_SIZE)
    {
        // still more to come
        return Continue;
    }

    // we have enough to interpret the frame size
    frameDesc.frameSize = interpretFrameSize(inputBuffer);
    WSVERBOSE("frameSize = %lld recv = %lld", frameDesc.frameSize, frameDesc.recvd);

    // frame indicates it is extended in size
    if(frameDesc.recvd < frameDesc.frameSize)
    {
        bytesRead = recv(sock, inputBuffer + frameDesc.recvd, frameDesc.frameSize - frameDesc.recvd, 0);
        switch (bytesRead) {
        case -1:
            if(errno == EWOULDBLOCK || errno == EAGAIN) {
                return Continue;
            }

            WSWARN("recvFrame extended errno = %d %s", errno, getErrorString(errBuf, sizeof(errBuf), errno));

            return NetworkError;
        case 0:
            return NetworkError;
        default:
            break;
        }

        frameDesc.recvd += bytesRead;
    }

    return Success;
}

ResultType recvPayload(websocket::socket sock,
                       bool masked,
                       const uint8_t mask[4],
                       void *pData,
                       ullong recvSize,
                       ullong bytesSoFar,
                       ullong &bytesRecvd)
{
    ssize_t bytesRead;
    char errBuf[512];

    bytesRecvd = 0;

    if(recvSize)
    {
        bytesRead = recv(sock, (uint8_t *)pData, recvSize, MSG_DONTWAIT);

        switch (bytesRead) {
        case -1:
            if(errno == EWOULDBLOCK || errno == EAGAIN) {
                return Continue;
            }

            WSWARN("recvPayload errno = %d, %s", errno, getErrorString(errBuf, sizeof(errBuf), errno));

            return NetworkError;
        case 0:
            return NetworkError;
        default:
            break;
        }

        bytesRecvd = (ullong)bytesRead;

        if(masked && bytesRecvd)
        {
            // unmask by XOR every byte of the payload with the next byte of the masking
            // key, wrapping at the end of the masking key
            ullong j = bytesSoFar % 4;
            uint8_t *pUnmask = (uint8_t *)pData;
            if(bytesRecvd > 0)
            {
                for(ullong i = 0; i < (ullong)bytesRecvd; i++)
                {
                    *pUnmask = *pUnmask ^ mask[j];
                    pUnmask++;

                    j++;
                    j %= 4;
                }
            }
        }
    }

    return Success;
}

void generateMask(uint8_t *pMask)
{
    std::vector<unsigned char> mask =  Random::next(4);

    memcpy(pMask, &mask[0], 4);
}

ResultType sendFrame(websocket::socket sock,
                     bool lastFrame,
                     uint8_t op,
                     const void *pData,
                     const uint8_t *pMask,
                     uint8_t *pSendBuffer,
                     ullong payloadLength,
                     ullong sendLimit)
{
    ullong frameLength = 0;
    ssize_t bytesSent;
    char errBuf[512];

    pSendBuffer[0] = lastFrame ? FIN_BITMASK : 0x0;
    pSendBuffer[0] |= (op & OPCODE_BITMASK);
    pSendBuffer[1] = pMask ? MASKING_BITMASK : 0x0;
    frameLength = 2;

    if(payloadLength < 126)
    {
        pSendBuffer[1] = pSendBuffer[1] | (payloadLength & PAYLOADSIZE_BITMASK);

        WSVERBOSE("Sending. Raw frame: %02x %02x %lld payload bytes", pSendBuffer[0], pSendBuffer[1], payloadLength);
    }
    else if(payloadLength <= 0xFFFF)
    {
        pSendBuffer[1] = pSendBuffer[1] | (126 & PAYLOADSIZE_BITMASK);

        pSendBuffer[2] = (payloadLength & 0xFF00) >> 8;
        pSendBuffer[3] = (payloadLength & 0x00FF);
        frameLength += 2;

        WSVERBOSE("Sending. Raw frame: %02x %02x %02x %02x %lld payload bytes", pSendBuffer[0], pSendBuffer[1], pSendBuffer[2], pSendBuffer[3], payloadLength);
    }
    else
    {
        pSendBuffer[1] = pSendBuffer[1] | (127 & PAYLOADSIZE_BITMASK);

        pSendBuffer[2] = (uint8_t)((payloadLength & 0xFF00000000000000LL) >> 56);
        pSendBuffer[3] = (uint8_t)((payloadLength & 0x00FF000000000000LL) >> 48);
        pSendBuffer[4] = (uint8_t)((payloadLength & 0x0000FF0000000000LL) >> 40);
        pSendBuffer[5] = (uint8_t)((payloadLength & 0x000000FF00000000LL) >> 32);
        pSendBuffer[6] = (uint8_t)((payloadLength & 0x00000000FF000000LL) >> 24);
        pSendBuffer[7] = (uint8_t)((payloadLength & 0x0000000000FF0000LL) >> 16);
        pSendBuffer[8] = (uint8_t)((payloadLength & 0x000000000000FF00LL) >>  8);
        pSendBuffer[9] = (uint8_t)((payloadLength & 0x00000000000000FFLL) >>  0);
        frameLength += 8;

        WSVERBOSE("Sending. Raw frame: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %lld payload bytes",
                  pSendBuffer[0], pSendBuffer[1], pSendBuffer[2], pSendBuffer[3], pSendBuffer[4], pSendBuffer[5], pSendBuffer[6], pSendBuffer[7], pSendBuffer[8], pSendBuffer[9], payloadLength);
    }

    if(pMask)
    {
        pSendBuffer[frameLength++] = pMask[0];
        pSendBuffer[frameLength++] = pMask[1];
        pSendBuffer[frameLength++] = pMask[2];
        pSendBuffer[frameLength++] = pMask[3];
    }

    ullong totalLength = frameLength + payloadLength;
    ullong totalSent = 0;

    ullong frameLeft = frameLength;
    ullong payloadSent = 0;
    ullong bytesLeft = totalLength;

    // The first time through we have to send the frame and however much of the payload (possibly all of it) fits.
    // Subsequent times through we are sending only payload
    while(totalSent < totalLength)
    {
        ullong bytesToSend = sendLimit > bytesLeft ? bytesLeft : sendLimit;
        uint8_t *pNextPayload = (uint8_t *)pData + payloadSent;

        if(pNextPayload)
        {
            if(pMask)
            {
                for(ullong i = 0; i < (bytesToSend - frameLeft); i++)
                {
                    pSendBuffer[frameLeft + i] = pNextPayload[i] ^ pMask[(payloadSent + i) % 4];
                }
            }
            else
            {
                memcpy(pSendBuffer + frameLeft, pNextPayload, (bytesToSend - frameLeft));
            }
        }

        bytesSent = send(sock, pSendBuffer, bytesToSend, 0);
        if(bytesSent < 0)
        {
            bytesSent = 0;

            if(errno == EWOULDBLOCK ||
               errno == EAGAIN ||
               errno == EINPROGRESS)
            {
                WSVERBOSE("Sending payload: Only sent %zu of %llu bytes. errno = %d %s",
                          payloadSent,
                          payloadLength,
                          errno,
                          getErrorString(errBuf, sizeof(errBuf), errno));
            }
            else
            {
                WSWARN("Error sending payload: errno = %d %s", errno, getErrorString(errBuf, sizeof(errBuf), errno));

                return NetworkError;
            }
        }

        payloadSent += (bytesSent - frameLeft);
        frameLeft = 0;
        bytesLeft -= bytesSent;
        totalSent += bytesSent;
    }

    return Success;
}

ResultType sendCloseFrame(websocket::socket sock,
                          uint16_t closeCode,
                          const void *pData,
                          const uint8_t *pMask,
                          uint8_t *pSendBuffer,
                          ullong payloadLength,
                          ullong sendLimit)
{
    ullong newPayloadLength = payloadLength > 123 ? 123 : payloadLength;
    uint8_t buffer[2 + 123];

    buffer[0] = (uint8_t)((closeCode & 0xFF00) >> 8);
    buffer[1] = (uint8_t)((closeCode & 0x00FF));

    memcpy(&buffer[2], pData, newPayloadLength);

    return sendFrame(sock,
                     true,
                     CLOSE_OP,
                     buffer,
                     pMask,
                     pSendBuffer,
                     2 + newPayloadLength,
                     sendLimit);
}
/*
void sendPingFrame(websocket::socket sock, const void *pData, const uint8_t *pMask, uint8_t *pSendBuffer, ullong payloadLength, ullong sendLimit)
{
    sendFrame(sock, true, PING_OP, pData, pMask, pSendBuffer, payloadLength, sendLimit);
}
*/
void sendPongFrame(websocket::socket sock, const void *pData, const uint8_t *pMask, uint8_t *pSendBuffer, ullong payloadLength, ullong sendLimit)
{
    sendFrame(sock,
              true,
              PONG_OP,
              pData,
              pMask,
              pSendBuffer,
              payloadLength,
              sendLimit);
}

const char * getErrorString(char *buf, int maxLen, int error)
{
    const char *pErrorStr = strerror(error);

    if(pErrorStr)
    {
        strncpy(buf, pErrorStr, maxLen);
    }
    else
    {
        strncpy(buf, "???", maxLen);
    }

    return buf;
}

// --
// Begin UTF-8 validation

bool isValidUTF8(uint32_t state)
{
    uint32_t code;
    bool valid = true;

    if((state & 0xf0000000) == 0xf0000000)
    {
        code =  (state & 0x0000003f);
        code |= (state & 0x00003f00) >> 2;
        code |= (state & 0x003f0000) >> 4;
        code |= (state & 0x07000000) >> 6;

        valid = (code >= 0x10000) && (code <= 0x10ffff);
    }
    else if((state & 0x00f00000) == 0x00e00000)
    {
        code =  (state & 0x0000003f);
        code |= (state & 0x00003f00) >> 2;
        code |= (state & 0x000f0000) >> 4;

        valid = (code >= 0x800) && (code <= 0xffff);
    }
    else if((state & 0x0000e000) == 0x0000c000)
    {
        code =  (state & 0x0000003f);
        code |= (state & 0x00001f00) >> 2;

        valid = (code >= 0x80) && (code <= 0x7ff);
    }
    else
    {
        code = state;

        valid = (code <= 0x7f);
    }

    if(valid)
    {
        if((code >= 0xd800) && (code <= 0xdfff))
        {
            return false;
        }
    }

    return valid;
}

int startUTF8(uint8_t c, uint32_t &state)
{
    state = c;

    if(!(c & 0x80))
    {
        return 0;
    }
    else if((c & 0xc0) == 0x80)
    {
        return -3;
    }
    else if((c & 0xe0) == 0xc0)
    {
        if(c == 0xc0 || c == 0xc1)
        {
            return -4;
        }

        return 1;
    }
    else if((c & 0xf0) == 0xe0)
    {
        return 2;
    }
    else if(c > 0xf4)
    {
        return -5;
    }
    else if((c & 0xf8) == 0xf0)
    {
        return 3;
    }

    return -6;
}

int continueUTF8(uint8_t c, int expected, uint32_t &state)
{
    // continuation bytes have the high bit set
    if((c & 0xc0) != 0x80)
    {
        return -6;
    }

    if(c & 0x40)
    {
        return -999;
    }

    state =  state << 8;
    state |= c;

    return expected - 1;
}

int validateUTF8(uint8_t *pData, ullong length, int expected, uint32_t &state)
{
    for(uint8_t *c = pData; (ullong)(c - pData) < length; c++)
    {
        if(expected == 0)
        {
            expected = startUTF8(*c, state);
        }
        else
        {
            expected = continueUTF8(*c, expected, state);
        }

        if(expected == 0)
        {
            if(!isValidUTF8(state))
            {
                return -12;
            }
        }

        if(expected < 0)
        {
            break;
        }
    }

    return expected;
}

// End private functions
//--
