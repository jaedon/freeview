/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef EventSourceEventConnection_h
#define EventSourceEventConnection_h

#include <nrd/config.h>
#include <nrd/EventConnection.h>
#include <nrdbase/Mutex.h>

#ifdef NRDP_WEBSERVER_ENABLED
extern "C" {
#include <mongoose.h>
}

namespace netflix {

class EventSourceEventConnection : public EventConnection
{
public:
    EventSourceEventConnection(struct mg_connection *conn, const struct mg_request_info *requestInfo);
    virtual ~EventSourceEventConnection();

    virtual void wait();
    virtual void finish();

    virtual bool writeHeader();
    virtual bool writeData(const std::string& data);

    struct mg_connection* connection() const {
        return mConnection;
    }
    const struct mg_request_info* request_info() const {
        return mRequestInfo;
    }

protected:
    virtual bool writeResponse(const Response &response);

private:
    int mWakeup[2];
    struct mg_connection* mConnection;
    const struct mg_request_info* mRequestInfo;
    Mutex mMutex;
    unsigned int mJsonFlags;
};

}

#endif

#endif
