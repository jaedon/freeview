/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "EventSourceEventConnection.h"
#ifdef NRDP_WEBSERVER_ENABLED
#include <nrd/NrdApplication.h>
#include <nrdbase/JSONFormatter.h>
#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Time.h>

#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace netflix;

EventSourceEventConnection::EventSourceEventConnection(struct mg_connection *conn, const struct mg_request_info *requestInfo)
    : EventConnection("EventSource"), mConnection(conn), mRequestInfo(requestInfo), mMutex(EVENTSOURCEEC_MUTEX, "EventSourceEventConnection"), mJsonFlags(0)
{
    if (pipe(mWakeup) == -1)
        mWakeup[0] = mWakeup[1] = -1;

    const Variant query = Variant::fromQueryString(mRequestInfo->query_string);

    bool hasFilter;
    const std::string filter = query.mapValue<std::string>("objectFilter", &hasFilter);
    if (hasFilter) {
        if (const int len = filter.length()) {
            const char *filter_str = filter.c_str();
            int last = 0;
            std::set<std::string> in, out;
            for (int i=0; i<len; ++i) {
                if (filter_str[i] == '|' || filter_str[i] == ',' || i + 1 == len) {
                    if (i + 1 == len)
                        ++i;
                    std::string objName(filter_str + last, i - last);
                    if (objName[0] == '-')
                        out.insert(objName.substr(1));
                    else
                        in.insert(objName);
                    last = i + 1;
                }
            }
            setFilters(in, out);
        }
    }
    setName(query.mapValue<std::string>("name", 0, "EventSource"));
    if (query.mapValue<bool>("pretty", 0, false))
        mJsonFlags |= JSONFormatter::Pretty;
}

EventSourceEventConnection::~EventSourceEventConnection()
{
    close(mWakeup[0]);
    close(mWakeup[1]);
}

bool EventSourceEventConnection::writeResponse(const Response &response)
{
    JSONFormatter formatter(mJsonFlags);
    return writeData(formatter.format(response.toVariant()));
}

void EventSourceEventConnection::wait()
{
    const int fd = mg_get_socket(mConnection);
    while (1) {
        fd_set r;
        FD_ZERO(&r);
        FD_SET(fd, &r);
        FD_SET(mWakeup[0], &r);
        if (select(std::max(mWakeup[0], fd) + 1, &r, 0, 0, 0) > 0) {
            if (FD_ISSET(mWakeup[0], &r)) {
                break;
            } else if (FD_ISSET(fd, &r)) {
                char buf;
                if (recv(fd, &buf, sizeof(buf), MSG_PEEK | MSG_DONTWAIT) == 0) {
                    break;
                }
            }
        } else if (errno != EINTR) {
            break;
        }
    }
}

void EventSourceEventConnection::finish()
{
    const char data = 'x';
    if (::write(mWakeup[1], &data, 1) == -1) {
        Log::error(TRACE_EVENTCONNECTION, "Unable to finish EventSourceEventConnection (%d)", errno);
    }
}

bool EventSourceEventConnection::writeHeader()
{
    // NTRACE(TRACE_EVENTCONNECTION, "Wrote header to EventSourceEventConnection %s (filter %0x)",
    //        header, filter());
    return NrdApplication::writeDataF(connection(),
                                      "HTTP/1.1 200 OK\r\n"
                                      "Cache: no-cache\r\n"
                                      "Cache-Control: private\r\n"
                                      "Pragma: no-cache\r\n"
                                      "Date: %s\r\n"
                                      "Content-Type: text/event-stream\r\n"
                                      "\r\n", Time::serverTime().calendarTimeText().c_str());
}

bool EventSourceEventConnection::writeData(const std::string& data)
{
    ScopedMutex lock(mMutex);

    const int length = data.length();
    if (mJsonFlags & JSONFormatter::Pretty) {
        return NrdApplication::writeData(connection(), data.c_str(), length) && NrdApplication::writeData(connection(), "\n", 1);
    } else {
        return (NrdApplication::writeData(connection(), "data:", 5)
                && NrdApplication::writeData(connection(), data.c_str(), length)
                && NrdApplication::writeData(connection(), "\r\n\r\n", 4));
    }
}
#endif
