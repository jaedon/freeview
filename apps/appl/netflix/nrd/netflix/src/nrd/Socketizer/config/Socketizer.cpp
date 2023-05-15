/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Socketizer.h"
#include "Socketizer_p.h"
#include "SocketizerIPAddress.h"
#include "SocketizerLog.h"
#include <list>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <dlfcn.h>
#include <stdint.h>
#include <time.h>
#include <algorithm>

using namespace netflix::socketizer;

Mutex SocketizerPrivate::sMutex;
Socketizer* Socketizer::sInstance = 0;

Socketizer::Socketizer()
    : mPrivate(new SocketizerPrivate)
{
}

Socketizer::~Socketizer()
{
    delete mPrivate;
}

static inline std::string lowerString(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

void Socketizer::setHostName(const std::string& name, const std::string& reverseName,
                             const std::vector<std::string>& aliases, const std::string& addr)
{
    IPAddress a(addr);
    if (a.type() == IPAddress::Invalid)
        return;
    MutexLocker locker(mPrivate->mMutex);

    std::string host;
    std::set<std::string> hosts;
    hosts.insert(lowerString(name));
    hosts.insert(lowerString(reverseName));
    std::vector<std::string>::const_iterator it = aliases.begin();
    const std::vector<std::string>::const_iterator end = aliases.end();
    while (it != end) {
        hosts.insert(lowerString(*it));
        ++it;
    }
    mPrivate->mHostNames[a] = hosts;
}

void Socketizer::enableLogging(bool enable)
{
    Log::enable(enable);
}

Socketizer* Socketizer::instance()
{
    MutexLocker locker(SocketizerPrivate::sMutex);
    if (!sInstance)
        sInstance = new Socketizer;
    return sInstance;
}

void Socketizer::addCondition(const Condition& condition)
{
    MutexLocker locker(mPrivate->mMutex);
    mPrivate->mConditions.push_back(condition);
}

void Socketizer::addCondition(Call call, const Condition& condition)
{
    static SocketizerPrivate::Data* data = mPrivate->findData(call);
    MutexLocker locker(*data->mutex);
    data->conditions.push_back(condition);
}

bool Socketizer::addCondition(const std::string& host, const Condition& condition)
{
    // is 'host' an IP address?
    IPAddress ip(host);
    if (ip.type() == IPAddress::Invalid) {
        // no, assume we're passing a real hostname
        Condition cond = condition;
        cond.flags |= Condition::Host;
        cond.host = host;
        MutexLocker locker(mPrivate->mMutex);
        mPrivate->mConditions.push_back(cond);
        return true;
    }
    MutexLocker locker(mPrivate->mMutex);
    mPrivate->mIPConditions[ip].push_back(condition);
    return true;
}

bool Socketizer::addCondition(const std::string& host, Call call, const Condition& condition)
{
    // is 'host' an IP address?
    IPAddress ip(host);
    if (ip.type() == IPAddress::Invalid) {
        // no, assume we're passing a real hostname
        Condition cond = condition;
        cond.flags |= Condition::Host;
        cond.host = host;
        static SocketizerPrivate::Data* data = mPrivate->findData(call);
        MutexLocker locker(*data->mutex);
        data->conditions.push_back(cond);
        return true;
    }
    static SocketizerPrivate::Data* data = mPrivate->findData(call);
    MutexLocker locker(*data->mutex);
    data->ipConditions[ip].push_back(condition);
    return true;
}

static const char* names[] = {
    "socket", "bind", "getsockopt", "setsockopt", "connect",
    "close", "send", "recv", "sendto", "recvfrom", "sendmsg",
    "recvmsg", "read", "write", "select", "accept", "open",
    "pipe", 0
};

Socketizer::Call Socketizer::stringToCall(const std::string& call, bool* ok)
{
    if (ok)
        *ok = true;
    // see if 'call' parses as an int and is >= 0 && < Socketizer::Last
    char* end;
    const unsigned long val = strtoul(call.c_str(), &end, 10);
    if (end == call.c_str() + call.size() && val < Socketizer::Last) {
        return static_cast<Socketizer::Call>(val);
    }

    for (int i=0; names[i]; ++i) {
        if (call == names[i])
            return static_cast<Socketizer::Call>(i);
    }
    if (ok)
        *ok = false;
    return Socketizer::Last;
}

SocketizerPrivate::~SocketizerPrivate()
{
    for (std::vector<Data*>::const_iterator it = mData.begin(); it != mData.end(); ++it) {
        delete *it;
    }
}

const char* SocketizerPrivate::callName(Socketizer::Call call)
{
    assert(call != Socketizer::Last);
    return names[call];
}

void SocketizerPrivate::initPtr(Socketizer::Call call)
{
    assert(call < mData.size());
    Data *&d = mData[call];
    if (!d)
        d = new Data;
    d->ptr = dlsym(RTLD_NEXT, callName(call));
}

SocketizerPrivate::Data* SocketizerPrivate::findData(Socketizer::Call call)
{
    MutexLocker locker(mMutex);
    if (mData.empty()) {
        srandom(time(0));
        mData.resize(Socketizer::Last);
        const Socketizer::Call inits[] = {
            Socketizer::Socket,
            Socketizer::Bind,
            Socketizer::GetSockOpt,
            Socketizer::SetSockOpt,
            Socketizer::Connect,
            Socketizer::Close,
            Socketizer::Send,
            Socketizer::Recv,
            Socketizer::SendTo,
            Socketizer::RecvFrom,
            Socketizer::SendMsg,
            Socketizer::RecvMsg,
            Socketizer::Read,
            Socketizer::Write,
            Socketizer::Select,
            Socketizer::Accept,
            // Socketizer::Open,
            // Socketizer::Pipe,
            Socketizer::Last
        };
        for (unsigned int i = 0; inits[i] != Socketizer::Last; ++i) {
            initPtr(inits[i]);
        }
    }
    assert(call < mData.size());
    return mData[call];
}
