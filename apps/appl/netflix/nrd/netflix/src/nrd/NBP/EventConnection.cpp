/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "EventConnection.h"

#include <nrdbase/JSONFormatter.h>
#include <nrdbase/Log.h>
using namespace netflix;

EventConnection::EventConnection(const std::string &name)
    : mName(name), mMutex(EVENTCONNECTION_MUTEX, "EventConnectionMutex"), mSyncComplete(false)
{
}

bool EventConnection::write(const Response &response)
{
    switch (response.type()) {
    case Request::Event:
    case Request::SetProperty:
    case Request::PropertiesUpdate:
    case Request::Method:
    case Request::ClassSync: {
        JSONFormatter formatter;
        formatter.setMaxStringLength(512);
        {
            ScopedMutex lock(mMutex);
            if (!mSyncComplete) {
                mPendingResponses.push_back(response);
                assert(mPendingResponses.size() < 200);
                lock.unlock();
                NTRACE(TRACE_EVENTCONNECTION, "Pushing to event connection backlog: %s (%s)",
                       mName.c_str(), formatter.format(response.toVariant()).c_str());
                return true;
            }
        }
        NTRACE(TRACE_EVENTCONNECTION, "Writing to eventConnection from write (%s): %s",
               mName.c_str(), formatter.format(response.toVariant()).c_str());

        if (!writeResponse(response)) {
            Log::warn(TRACE_EVENTCONNECTION,
                      "Failed to write %s to EventConnection (%s)",
                      formatter.format(response.toVariant()).c_str(), mName.c_str());
            return false;
        }
        return true; }
    case Request::Invalid:
    case Request::GetProperty:
        // should never happen
        break;
    }
    assert(0);
    return false;
}

bool EventConnection::sync(const std::vector<Response> &responses)
{
    JSONFormatter formatter;
    formatter.setMaxStringLength(512);
    for (std::vector<Response>::const_iterator it = responses.begin(); it != responses.end(); ++it) {
        const Response &response = *it;
        NTRACE(TRACE_EVENTCONNECTION, "Writing to eventConnection from write (%s): %s",
               mName.c_str(), formatter.format(response.toVariant()).c_str());
        if (!writeResponse(response)) {
            Log::warn(TRACE_EVENTCONNECTION,
                      "Failed to write %s to EventConnection (%s)",
                      formatter.format(response.toVariant()).c_str(), mName.c_str());
            return false;
        }
    }

    size_t idx = 0;
    while (true) {
        Response response;
        {
            ScopedMutex lock(mMutex);
            if (idx == mPendingResponses.size()) {
                mPendingResponses.clear();
                mSyncComplete = true;
                break;
            }
            response = mPendingResponses[idx++];
        }
        if (!writeResponse(response)) {
            NTRACE(TRACE_EVENTCONNECTION, "Writing to eventConnection from finishSync (%s): %s",
                   mName.c_str(), formatter.format(response.toVariant()).c_str());
            Log::warn(TRACE_EVENTCONNECTION,
                      "Failed to write %s to EventSourceEventConnection from finishSync (%s)",
                      formatter.format(response.toVariant()).c_str(), mName.c_str());
            return false;
        }
    }
    return true;
}

