/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SOCKETIZER_P_H
#define SOCKETIZER_P_H

#include <list>
#include <map>
#include <set>
#include <vector>
#include "SocketizerMutex.h"
#include "SocketizerIPAddress.h"

namespace netflix {
namespace socketizer {

class SocketizerPrivate
{
public:
    ~SocketizerPrivate();

    class Data
    {
    public:
        Data()
            : ptr(0), mutex(new Mutex)
        {}
        ~Data()
        {
            delete mutex;
        }
        void* ptr;
        Mutex *mutex;
        std::list<Socketizer::Condition> conditions;
        std::map<IPAddress, std::list<Socketizer::Condition> > ipConditions;
    private:
        Data(const Data &);
        Data &operator=(const Data &);
    };
    struct Socket
    {
        Socket()
            : domain(-1), type(-1), protocol(-1), state(Invalid), when(0)
        {
        }

        int domain, type, protocol;
        IPAddress address;
        std::set<std::string> hosts;

        enum {
            Invalid,
            Normal,
            InProgress,
            PendingError
        } state;

        int err;
        uint64_t when;
    };

    void initPtr(Socketizer::Call call);
    Data* findData(Socketizer::Call call);
    static const char* callName(Socketizer::Call call);

    static Mutex sMutex;

    Mutex mMutex;
    std::vector<Data*> mData;
    std::map<int, Socket> mSockets;
    std::map<IPAddress, std::list<Socketizer::Condition> > mIPConditions;
    std::list<Socketizer::Condition> mConditions;
    std::map<IPAddress, std::set<std::string> > mHostNames;

    static SocketizerPrivate* priv(Socketizer* socketizer)
    {
        return socketizer->mPrivate;
    }
};
}
}

#endif
