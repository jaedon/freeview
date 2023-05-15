/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SOCKETIZER_H
#define SOCKETIZER_H

#include <map>
#include <stdio.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

namespace netflix {
namespace socketizer {
class SocketizerPrivate;

class Socketizer
{
public:
    ~Socketizer();

    static Socketizer* instance();

    void initializeLogging(const FILE* file);
    void enableLogging(bool enable);

    enum Call { Socket, Bind, GetSockOpt, SetSockOpt, Connect, Close, Send, Recv, SendTo,
                RecvFrom, SendMsg, RecvMsg, Read, Write, Select, Accept, Open, Pipe, Last };

    void setHostName(const std::string& name, const std::string& reverseName,
                     const std::vector<std::string>& aliases, const std::string& addr);

    class Condition
    {
    public:
        enum Flags {
            Host             = 0x001,
            Probability      = 0x002,
            Timeout          = 0x004,
            Block            = 0x008,
            Errno            = 0x010,
            Count            = 0x020,
            CountProbability = 0x040, // decrease count only when the probability succeeds
            FileDescriptor   = 0x080,
            Domain           = 0x100,
            Type             = 0x200,
            Protocol         = 0x400,
            Pattern          = 0x800
        };

        enum { Forever = 0 };

        Condition()
            : flags(0)
        {
        }

        unsigned int flags;
        float probability; // 0 - 1
        std::string host; // can include wildcards
        unsigned int timeout, block; // ms
        int err;
        int fd;
        int domain, type, protocol;

        struct {
            std::vector<bool> match;
            int repeat; // -1 = forever
        } pattern;

        int count; // 0 means forever
    };
    void addCondition(const Condition& condition);
    void addCondition(Call call, const Condition& condition);
    bool addCondition(const std::string& host, const Condition& condition);
    bool addCondition(const std::string& host, Call call, const Condition& condition);

    static Call stringToCall(const std::string& call, bool* ok = 0);

private:
    Socketizer();

    static Socketizer* sInstance;

    SocketizerPrivate* mPrivate;
    friend class SocketizerPrivate;
};
}
}

#endif
