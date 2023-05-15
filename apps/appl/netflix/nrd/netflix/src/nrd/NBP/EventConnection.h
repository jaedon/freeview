/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef EventConnection_h
#define EventConnection_h

#include <string>
#include <nrdbase/Mutex.h>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/ScopedMutex.h>
#include <nrd/Response.h>

namespace netflix {
class NfObject;
class EventConnection : public Noncopyable
{
public:
    EventConnection(const std::string &name = "Unnamed");
    virtual ~EventConnection() {}

    virtual void finish() {}
    bool write(const Response &response);

    inline std::string name() const { return mName; }

    inline bool isFiltered(const std::string &object) const
    {
        if (object.empty())
            return false;
        if (mFilterOut.find(object) != mFilterOut.end())
            return true;
        if (mFilterIn.empty())
            return false;
        return mFilterIn.find(object) == mFilterIn.end();
    }
    bool sync(const std::vector<Response> &responses);
protected:
    inline void setName(const std::string &name)
    {
        mName = name;
    }
    inline void setFilters(const std::set<std::string> &in, const std::set<std::string> &out)
    {
        mFilterIn = in;
        mFilterOut = out;
    }

    virtual bool writeResponse(const Response &response) = 0;

private:
    std::string mName;
    std::set<std::string> mFilterIn, mFilterOut;
    Mutex mMutex;
    std::vector<Response> mPendingResponses;
    bool mSyncComplete;
};
}

#endif
