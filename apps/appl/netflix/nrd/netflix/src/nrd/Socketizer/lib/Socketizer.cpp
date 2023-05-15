/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <config/Socketizer.h>
#include <config/Socketizer_p.h>
#include <config/SocketizerLog.h>
#include <algorithm>
#include <fnmatch.h>
#include <unistd.h>
#include <assert.h>
#include <execinfo.h>

using namespace netflix::socketizer;

#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
#include <mach/clock.h>
#include <mach/mach.h>

void clock_gettime(int /*clock_id*/, timespec* ts)
{
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), REALTIME_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;
}

#define CLOCK_MONOTONIC 0
#endif


static Mutex patternMutex;
std::map<std::string, unsigned int> patternCount;

// need to have SocketizerPrivate::Mutex locked
static inline std::set<std::string> hostNames(const IPAddress& addr)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    std::map<IPAddress, std::set<std::string> >::const_iterator it = priv->mHostNames.find(addr);
    if (it == priv->mHostNames.end())
        return std::set<std::string>();
    return it->second;
}

static inline uint64_t currentTime()
{
    timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    const uint64_t res = (spec.tv_sec * 1000) + (spec.tv_nsec / 1000000);
    return res;
}

static inline bool matchCondition(Socketizer::Condition& condition,
                                  const std::set<std::string>& hosts, int fd,
                                  int domain, int type, int protocol)
{
    if ((condition.flags & Socketizer::Condition::FileDescriptor) && fd != -1 && fd != condition.fd)
        return false;
    if ((condition.flags & Socketizer::Condition::Domain) && domain != -1 && domain != condition.domain)
        return false;
    if ((condition.flags & Socketizer::Condition::Type) && type != -1 && type != condition.type)
        return false;
    if ((condition.flags & Socketizer::Condition::Protocol) && protocol != -1 && protocol != condition.protocol)
        return false;
    if ((condition.flags & Socketizer::Condition::Host) && !condition.host.empty()) {
        if (hosts.empty())
            return false;

        // glob
        std::string pattern = condition.host;
        std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);

        std::set<std::string>::const_iterator it = hosts.begin();
        const std::set<std::string>::const_iterator end = hosts.end();
        while (it != end) {
            std::string match = *it;
            if (fnmatch(pattern.c_str(), match.c_str(), FNM_NOESCAPE) == 0) {
                if (condition.flags & Socketizer::Condition::Pattern) {
                    MutexLocker patternLocker(patternMutex);
                    unsigned int& cnt = patternCount[match];
                    if (condition.pattern.match.size() <= cnt)
                        return false;
                    const bool matched = condition.pattern.match[cnt];
                    ++cnt;
                    if (cnt >= condition.pattern.match.size() && condition.pattern.repeat) {
                        if (condition.pattern.repeat > 0)
                            --condition.pattern.repeat;
                        cnt = 0;
                    }
                    return matched;
                }

                return true;
            }
            ++it;
        }
        return false;
    }
    return true;
}

static inline std::list<Socketizer::Condition>::iterator matchCondition(std::list<Socketizer::Condition>& conditions,
                                                                        const std::set<std::string>& hosts,
                                                                        int fd = -1,
                                                                        int domain = -1,
                                                                        int type = -1,
                                                                        int protocol = -1)
{
    std::list<Socketizer::Condition>::iterator it = conditions.begin();
    const std::list<Socketizer::Condition>::const_iterator end = conditions.end();
    while (it != end) {
        if (matchCondition(*it, hosts, fd, domain, type, protocol)) {
            const bool count = it->flags & Socketizer::Condition::Count;
            // check probability
            if (it->flags & Socketizer::Condition::Probability) {
                const float prob = static_cast<double>(random()) / RAND_MAX;
                if (prob <= it->probability) {
                    // hit
                    return it;
                }
                if (!count || (count && !(it->flags & Socketizer::Condition::CountProbability))) {
                    if (it->count > 0 && !--it->count) {
                        // we're done, take it out
                        it = conditions.erase(it);
                    }
                }
                return conditions.end();
            }
            // hit
            return it;
        }
        ++it;
    }
    return conditions.end();
}

static inline int processCondition(std::list<Socketizer::Condition>::iterator& condition,
                                   std::list<Socketizer::Condition>& conditions, bool& ok)
{
    ok = false;
    int ret = 0;
    if (condition->flags & Socketizer::Condition::Errno) {
        ret = condition->err;
        ok = true;
    } else if (condition->flags & Socketizer::Condition::Timeout) {
        ret = EINPROGRESS;
        ok = true;
    }
    if (condition->flags & Socketizer::Condition::Block) {
        // how long should we block?
        if (condition->block > 0) {
            usleep(condition->block * 1000);
        }
    }
    const bool count = condition->flags & Socketizer::Condition::Count;
    if (!count || (condition->count > 0 && !--condition->count)) {
        conditions.erase(condition);
    }
    return ret;
}

static inline void updateTimeout(SocketizerPrivate* priv, int fd, const Socketizer::Condition& cnd)
{
    MutexLocker locker(priv->mMutex);
    assert(fd != -1);
    assert(priv->mSockets.find(fd) != priv->mSockets.end());
    SocketizerPrivate::Socket& socket = priv->mSockets[fd];
    socket.state = SocketizerPrivate::Socket::InProgress;
    socket.when = currentTime() + cnd.timeout;
    socket.err = (cnd.flags & Socketizer::Condition::Errno) ? cnd.err : 0;
}

static inline bool checkConditions(SocketizerPrivate* priv, SocketizerPrivate::Data* data,
                                   const sockaddr* addr, int fd, int domain, int type, int protocol, int* ret)
{
    assert(ret);

    IPAddress address;
    std::set<std::string> hosts;
    bool ok;

    std::list<Socketizer::Condition>::iterator it;
    {
        MutexLocker locker(priv->mMutex);
        if (fd != -1) {
            if (priv->mSockets.find(fd) == priv->mSockets.end())
                return false;
            SocketizerPrivate::Socket& socket = priv->mSockets[fd];
            hosts = socket.hosts;
            if (addr)
                address = IPAddress(addr);
            else
                address = socket.address;
        }

        if (address.type() != IPAddress::Invalid) {
            std::map<IPAddress, std::list<Socketizer::Condition> >::iterator ipcond = priv->mIPConditions.find(address);
            if (ipcond != priv->mIPConditions.end()) {
                it = matchCondition(ipcond->second, hosts, fd, domain, type, protocol);
                if (it != ipcond->second.end()) {
                    const Socketizer::Condition cnd = *it;
                    *ret = processCondition(it, ipcond->second, ok);
                    if (cnd.flags & Socketizer::Condition::Timeout && *ret == EINPROGRESS) {
                        locker.unlock();
                        updateTimeout(priv, fd, cnd);
                    }
                    return ok;
                }
            }
        }

        it = matchCondition(priv->mConditions, hosts, fd, domain, type, protocol);
        if (it != priv->mConditions.end()) {
            const Socketizer::Condition cnd = *it;
            *ret = processCondition(it, priv->mConditions, ok);
            if (cnd.flags & Socketizer::Condition::Timeout && *ret == EINPROGRESS) {
                locker.unlock();
                updateTimeout(priv, fd, cnd);
            }
            return ok;
        }
    }
    {
        MutexLocker locker(*data->mutex);
        if (address.type() != IPAddress::Invalid) {
            std::map<IPAddress, std::list<Socketizer::Condition> >::iterator ipcond = data->ipConditions.find(address);
            if (ipcond != data->ipConditions.end()) {
                it = matchCondition(ipcond->second, hosts, fd, domain, type, protocol);
                if (it != ipcond->second.end()) {
                    const Socketizer::Condition cnd = *it;
                    *ret = processCondition(it, ipcond->second, ok);
                    if (cnd.flags & Socketizer::Condition::Timeout && *ret == EINPROGRESS) {
                        locker.unlock();
                        updateTimeout(priv, fd, cnd);
                    }
                    return ok;
                }
            }
        }

        it = matchCondition(data->conditions, hosts, fd, domain, type, protocol);
        if (it != data->conditions.end()) {
            const Socketizer::Condition cnd = *it;
            *ret = processCondition(it, data->conditions, ok);
            if (cnd.flags & Socketizer::Condition::Timeout && *ret == EINPROGRESS) {
                locker.unlock();
                updateTimeout(priv, fd, cnd);
            }
            return ok;
        }
    }
    return false;
}

typedef int (*SocketPtr)(int, int, int);
typedef int (*BindPtr)(int, const sockaddr*, socklen_t);
typedef int (*GetSockOptPtr)(int, int, int, void*, socklen_t*);
typedef int (*SetSockOptPtr)(int, int, int, const void*, socklen_t);
typedef int (*ConnectPtr)(int, const sockaddr*, socklen_t);
typedef int (*ClosePtr)(int);
typedef ssize_t (*SendPtr)(int, const void*, size_t, int);
typedef ssize_t (*SendToPtr)(int, const void*, size_t, int, const sockaddr*, socklen_t);
typedef ssize_t (*SendMsgPtr)(int, const struct msghdr*, int);
typedef ssize_t (*RecvPtr)(int, void*, size_t, int);
typedef ssize_t (*RecvFromPtr)(int, void*, size_t, int, sockaddr*, socklen_t*);
typedef ssize_t (*RecvMsgPtr)(int, struct msghdr*, int);
typedef int (*SelectPtr)(int, fd_set*, fd_set*, fd_set*, struct timeval*);
typedef ssize_t (*ReadPtr)(int fd, void *buf, size_t count);
typedef ssize_t (*WritePtr)(int fd, const void *buf, size_t count);
typedef int (*AcceptPtr)(int, struct sockaddr*, socklen_t*);
typedef int (*OpenPtr)(const char *, int, ...);
typedef int (*PipePtr)(int [2]);

extern "C" {

int socket(int domain, int type, int protocol)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::Socket);
    log() << "socket" << domain << type << protocol;

    // check for conditions
    int ret;
    if (checkConditions(priv, data, 0, -1, domain, type, protocol, &ret)) {
        errno = ret;
        return -1;
    }

    ret = reinterpret_cast<SocketPtr>(data->ptr)(domain, type, protocol);
    if (ret != -1) {
        MutexLocker locker(priv->mMutex);
        assert(priv->mSockets.find(ret) == priv->mSockets.end());
        SocketizerPrivate::Socket& socket = priv->mSockets[ret];
        socket.domain = domain;
        socket.type = type;
        socket.protocol = protocol;
        socket.state = SocketizerPrivate::Socket::Normal;
        // char buf[1024];
        // snprintf(buf, sizeof(buf), "/tmp/sockets/%d", ret);
        // FILE *f = fopen(buf, "w");
        // fprintf(f, "open %d\n", priv->mSockets.size());
        // void *buffer[100];
        // unsigned size = ::backtrace(buffer, 100);
        // ::backtrace_symbols_fd(buffer, size, fileno(f));
        // fclose(f);
    }

    return ret;
}

int bind(int sockfd, const sockaddr *addr, socklen_t addrlen)
{
    static SocketizerPrivate::Data* data = SocketizerPrivate::priv(Socketizer::instance())->findData(Socketizer::Bind);
    log() << "bind";
    return reinterpret_cast<BindPtr>(data->ptr)(sockfd, addr, addrlen);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::Accept);
    log() << "accept";
    int ret;
    sockaddr* a = 0;
    sockaddr_storage as;
    socklen_t l = 0;
    if (addr && addrlen) {
        a = addr;
        l = *addrlen;
    } else {
        a = reinterpret_cast<sockaddr*>(&as);
        l = sizeof(sockaddr_storage);
    }
    ret = reinterpret_cast<AcceptPtr>(data->ptr)(sockfd, a, &l);
    if (ret != -1) {
        MutexLocker locker(priv->mMutex);
        assert(priv->mSockets.find(ret) == priv->mSockets.end());
        SocketizerPrivate::Socket& socket = priv->mSockets[ret];
        socket.state = SocketizerPrivate::Socket::Normal;
        socket.address = IPAddress(a);
        socket.hosts = hostNames(socket.address);
    }

    return ret;
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::GetSockOpt);
    log() << "getsockopt";
    if (level == SOL_SOCKET && optname == SO_ERROR) {
        if (!optlen || *optlen < sizeof(int)) {
            // bad
            errno = EINVAL;
            return -1;
        }

        // check if this is one of our in-progress sockets
        MutexLocker locker(priv->mMutex);
        assert(priv->mSockets.find(sockfd) != priv->mSockets.end());
        SocketizerPrivate::Socket& socket = priv->mSockets[sockfd];
        switch (socket.state) {
        case SocketizerPrivate::Socket::InProgress:
            // hang on a bit
            *optlen = sizeof(int);
            *reinterpret_cast<int*>(optval) = 0;
            return 0;
        case SocketizerPrivate::Socket::PendingError:
            // see if we have an error
            *optlen = sizeof(int);
            *reinterpret_cast<int*>(optval) = socket.err;
            return 0;
        default:
            break;
        }
    }
    int ret;
    if (checkConditions(priv, data, 0, sockfd, -1, -1, -1, &ret)) {
        errno = ret;
        return -1;
    }
    return reinterpret_cast<GetSockOptPtr>(data->ptr)(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::SetSockOpt);

    // check for conditions
    int ret;
    if (checkConditions(priv, data, 0, sockfd, -1, -1, -1, &ret)) {
        errno = ret;
        return -1;
    }

    log() << "setsockopt";
    return reinterpret_cast<SetSockOptPtr>(data->ptr)(sockfd, level, optname, optval, optlen);
}

int connect(int sockfd, const sockaddr *addr, socklen_t addrlen)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::Connect);

    {
        MutexLocker locker(priv->mMutex);
        assert(priv->mSockets.find(sockfd) != priv->mSockets.end());
        SocketizerPrivate::Socket& socket = priv->mSockets[sockfd];
        socket.address = IPAddress(addr);
        socket.hosts = hostNames(socket.address);
    }

    // check for conditions
    int ret;
    if (checkConditions(priv, data, 0, sockfd, -1, -1, -1, &ret)) {
        errno = ret;
        return -1;
    }

    log() << "connect" << sockfd;
    return reinterpret_cast<ConnectPtr>(data->ptr)(sockfd, addr, addrlen);
}

#if 0
int pipe(int out[2])
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::Pipe);
    log() << "pipe";

    const int ret = reinterpret_cast<PipePtr>(data->ptr)(out);
    MutexLocker locker(priv->mMutex);
    if (ret != -1) {
        for (int i=0; i<2; ++i) {
            assert(priv->mSockets.find(out[i]) == priv->mSockets.end());
            SocketizerPrivate::Socket& socket = priv->mSockets[out[i]];
            char buf[1024];
            snprintf(buf, sizeof(buf), "/tmp/sockets/%d", out[i]);
            FILE *f = fopen(buf, "w");
            fprintf(f, "pipe(%d) %d\n", i, priv->mSockets.size());
            void *buffer[100];
            unsigned size = ::backtrace(buffer, 100);
            ::backtrace_symbols_fd(buffer, size, fileno(f));
            fclose(f);
        }
    }
    return ret;
}

int open(const char *path, int oflag, ...)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::Open);
    log() << "open" << path << oflag;

    const int ret = reinterpret_cast<OpenPtr>(data->ptr)(path, oflag);
    MutexLocker locker(priv->mMutex);
    if (ret != -1 && priv->mSockets.find(ret) != priv->mSockets.end()) {
        if (priv->mSockets.find(ret) != priv->mSockets.end()) {
            printf("SHIT %s %d\n", path, ret);
        }
        assert(priv->mSockets.find(ret) == priv->mSockets.end());
        SocketizerPrivate::Socket& socket = priv->mSockets[ret];
        char buf[1024];
        snprintf(buf, sizeof(buf), "/tmp/sockets/%d", ret);
        FILE *f = fopen(buf, "w");
        fprintf(f, "open file %d\n", priv->mSockets.size());
        void *buffer[100];
        unsigned size = ::backtrace(buffer, 100);
        ::backtrace_symbols_fd(buffer, size, fileno(f));
        fclose(f);
    }

    return ret;
}

#endif

int close(int fd)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::Close);

    {
        MutexLocker locker(priv->mMutex);
        std::map<int, SocketizerPrivate::Socket>::iterator it = priv->mSockets.find(fd);
        if (it != priv->mSockets.end()) {
            log() << "close" << fd;

            // check for conditions
            int ret;
            locker.unlock();
            // char buf[1024];
            // snprintf(buf, sizeof(buf), "/tmp/sockets/%d", fd);
            // unlink(buf);

            if (checkConditions(priv, data, 0, fd, -1, -1, -1, &ret)) {
                locker.relock();
                priv->mSockets.erase(it);
                errno = ret;
                return -1;
            }
            locker.relock();
            priv->mSockets.erase(it);
        }
    }

    return reinterpret_cast<ClosePtr>(data->ptr)(fd);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::Send);

    // check for conditions
    int ret;
    if (checkConditions(priv, data, 0, sockfd, -1, -1, -1, &ret)) {
        errno = ret;
        return -1;
    }

    log() << "send" << sockfd;
    return reinterpret_cast<SendPtr>(data->ptr)(sockfd, buf, len, flags);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const sockaddr *dest_addr, socklen_t addrlen)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::SendTo);

    // check for conditions
    int ret;
    if (checkConditions(priv, data, dest_addr, sockfd, -1, -1, -1, &ret)) {
        errno = ret;
        return -1;
    }

    log() << "sendto" << sockfd;
    return reinterpret_cast<SendToPtr>(data->ptr)(sockfd, buf, len, flags, dest_addr, addrlen);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::SendMsg);

    // check for conditions
    int ret;
    if (checkConditions(priv, data, 0, sockfd, -1, -1, -1, &ret)) {
        errno = ret;
        return -1;
    }

    log() << "sendmsg" << sockfd;
    return reinterpret_cast<SendMsgPtr>(data->ptr)(sockfd, msg, flags);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::Recv);

    // check for conditions
    int ret;
    if (checkConditions(priv, data, 0, sockfd, -1, -1, -1, &ret)) {
        errno = ret;
        return -1;
    }

    log() << "recv" << sockfd;
    return reinterpret_cast<RecvPtr>(data->ptr)(sockfd, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t *addrlen)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::RecvFrom);

    // check for conditions
    int ret;
    if (checkConditions(priv, data, src_addr, sockfd, -1, -1, -1, &ret)) {
        errno = ret;
        return -1;
    }

    log() << "recvfrom" << sockfd;
    return reinterpret_cast<RecvFromPtr>(data->ptr)(sockfd, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::RecvMsg);

    // check for conditions
    int ret;
    if (checkConditions(priv, data, 0, sockfd, -1, -1, -1, &ret)) {
        errno = ret;
        return -1;
    }

    log() << "recvmsg" << sockfd;
    return reinterpret_cast<RecvMsgPtr>(data->ptr)(sockfd, msg, flags);
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::Select);
    // geh
    timeval timev;
    timeval* timeptr = timeout;
    uint64_t time = static_cast<uint64_t>(-1);
    std::map<int, SocketizerPrivate::Socket> sockets;
    if (writefds) {
        // check our in-progress sockets, add timeout
        {
            MutexLocker locker(priv->mMutex);
            std::map<int, SocketizerPrivate::Socket>::const_iterator it = priv->mSockets.begin();
            const std::map<int, SocketizerPrivate::Socket>::const_iterator end = priv->mSockets.end();
            while (it != end) {
                if (FD_ISSET(it->first, writefds) && it->second.state == SocketizerPrivate::Socket::InProgress) {
                    // calc timeout
                    sockets[it->first] = it->second;
                    if (time > it->second.when)
                        time = it->second.when;
                    // take it out of the set
                    FD_CLR(it->first, writefds);
                }
                ++it;
            }
        }
        if (!sockets.empty()) {
            if (!timeptr)
                timeptr = &timev;
            const uint64_t cur = currentTime();
            if (cur > time)
                time = 0;
            else
                time -= cur;
            timeptr->tv_sec = time / 1000;
            timeptr->tv_usec = (time % 1000) * 1000;
        }
    }
    log() << "select" << nfds;
    int ret = reinterpret_cast<SelectPtr>(data->ptr)(nfds, readfds, writefds, exceptfds, timeptr);
    if (ret >= 0 && !sockets.empty()) {
        assert(writefds);
        // check if one of our in-progess sockets hit
        const uint64_t cur = currentTime();
        std::map<int, SocketizerPrivate::Socket>::const_iterator it = sockets.begin();
        const std::map<int, SocketizerPrivate::Socket>::const_iterator end = sockets.end();
        while (it != end) {
            assert(it->second.state == SocketizerPrivate::Socket::InProgress);
            if (it->second.when <= cur) {
                // yes, but first verify that the socket is still really flagged
                MutexLocker locker(priv->mMutex);
                std::map<int, SocketizerPrivate::Socket>::iterator it2 = priv->mSockets.find(it->first);
                if (it2 == priv->mSockets.end()
                    || it2->second.state != SocketizerPrivate::Socket::InProgress
                    || it2->second.when > cur) {
                    ++it;
                    continue;
                }
                // we'll need to flag this socket
                assert(!FD_ISSET(it->first, writefds));
                FD_SET(it->first, writefds);
                ++ret;

                // mark this socket as available to get SO_ERROR
                it2->second.state = SocketizerPrivate::Socket::PendingError;
            }
            ++it;
        }
    }
    return ret;
}

ssize_t read(int fd, void *buf, size_t count)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::Read);
    {
        MutexLocker locker(priv->mMutex);
        std::map<int, SocketizerPrivate::Socket>::iterator it = priv->mSockets.find(fd);
        if (it != priv->mSockets.end()) {
            // check for conditions
            int ret;
            locker.unlock();
            if (checkConditions(priv, data, 0, fd, -1, -1, -1, &ret)) {
                errno = ret;
                return -1;
            }

            log() << "read" << fd;
        }
    }
    return reinterpret_cast<ReadPtr>(data->ptr)(fd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count)
{
    static SocketizerPrivate* priv = SocketizerPrivate::priv(Socketizer::instance());
    static SocketizerPrivate::Data* data = priv->findData(Socketizer::Write);
    {
        MutexLocker locker(priv->mMutex);
        std::map<int, SocketizerPrivate::Socket>::iterator it = priv->mSockets.find(fd);
        if (it != priv->mSockets.end()) {
            // check for conditions
            int ret;
            locker.unlock();
            if (checkConditions(priv, data, 0, fd, -1, -1, -1, &ret)) {
                errno = ret;
                return -1;
            }

            log() << "write" << fd;
        }
    }
    return reinterpret_cast<WritePtr>(data->ptr)(fd, buf, count);
}

} // extern "C"
