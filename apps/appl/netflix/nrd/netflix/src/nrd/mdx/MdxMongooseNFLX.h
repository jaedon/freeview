/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MdxMongooseNFLX_h
#define MdxMongooseNFLX_h

#include <stddef.h>
extern "C" {
// struct nflx_mg_context;
#include <mongoose_mdx.h>
}
#include <vector>
#include <nrdbase/Mutex.h>
#include <nrdbase/Noncopyable.h>
#include <nrdnet/WebSocketThread.h>

namespace netflix {
namespace mdx {

class MdxMongooseNFLX : public Noncopyable
{
public:
    MdxMongooseNFLX();
    ~MdxMongooseNFLX();

    static MdxMongooseNFLX *instance();

    static bool writeData(struct nflx_mg_connection *conn, const char *data, int length);
    static bool writeDataF(struct nflx_mg_connection *conn, const char *format, ...);
    static void sendMongooseResponse(std::string response, void * conndata);

    bool start(int listeningPort, bool enableWebSocket);
    void stop();
    bool isRunning() const;

    void setMaximumPostSize(unsigned int maxPostSizeBytes);
    unsigned int getMaximumPostSize() const;

    int getListeningPort() const;


    int mdxMongooseBeginRequest(struct nflx_mg_connection *conn);
    void mdxMongooseEndRequest(const struct nflx_mg_connection *conn, int reply_status_code);
    int mdxMongooseLogMessage(const struct nflx_mg_connection *conn, const char *message);
    void mdxMongooseTraceEventStart(int id, const char *message);
    void mdxMongooseTraceEventEnd(int id, const char *message);

private:
    void handlePing(struct nflx_mg_connection *conn);
    bool handleREST(struct nflx_mg_connection *conn, const struct nflx_mg_request_info *requestInfo);

    static MdxMongooseNFLX *sInstance;
    struct nflx_mg_context *mMongoose;
    unsigned int mMaximumPostSizeBytes;

    void runEchoServer(const websocket::ConnectionInfo & connectionInfo, const std::string &subprotocol);
    void runMdxServer(const websocket::ConnectionInfo &connectionInfo, const std::string &subprotocol);
    bool handleWebSocket(struct nflx_mg_connection *conn, const struct nflx_mg_request_info *requestInfo);

    shared_ptr<WebSocketThread> mWebSocketThread;
    int mListeningPort; // the port mongoose ended up listening on
};

}} //namespace netflix::mdx

#endif
