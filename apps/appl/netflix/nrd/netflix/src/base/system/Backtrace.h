
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef Backtrace_h
#define Backtrace_h

#include <nrdbase/tr1.h>
#include <stdlib.h>
#include <nrdbase/Noncopyable.h>
#include <sstream>
#include <nrdbase/config.h>
#include <string.h>
#include <unistd.h>
#include <iomanip>

#ifdef NRDP_HAS_EXECINFO_H
# include <execinfo.h>
#endif
#ifdef NRDP_HAS_CXA_DEMANGLE
#include <cxxabi.h>
#endif

namespace netflix {
class Backtrace : public Noncopyable
{
public:
    static void backtraceToFD(int fd)
    {
#ifdef NRDP_HAS_EXECINFO_H
        void *buffer[MAX_STACK_SIZE];
        unsigned size = ::backtrace(buffer, MAX_STACK_SIZE);
        ::backtrace_symbols_fd(buffer, size, fd);
        int result = fsync(fd);
        (void)result;
#else
        ssize_t bytes = write(fd, "ERROR: Backtrace not available\n", 31);
        (void)bytes;
#endif
    }

    static shared_ptr<Backtrace> backtrace(int maxFrames=MAX_STACK_SIZE)
    {
        shared_ptr<Backtrace> ret(new Backtrace);
#ifdef NRDP_HAS_EXECINFO_H
        ret->mSize = ::backtrace(ret->mStack, std::min(maxFrames, (int)MAX_STACK_SIZE));
#endif
        return ret;
    }

    ~Backtrace()
    {
        if (mSymbols)
            free(mSymbols);
#ifdef NRDP_HAS_CXA_DEMANGLE
        for (int i=0; i<MAX_STACK_SIZE; ++i) {
            if (mDemangled[i])
                free(mDemangled[i]);
        }
#endif
    }
    unsigned size() const { return mSize ? mSize - 1 : 0; }
    void *address(unsigned idx) const
    {
        if (idx + 1 >= mSize)
            return 0;
        return mStack[idx + 1];
    }
    const char *frame(unsigned idx) const
    {
        if (idx + 1 >= mSize)
            return 0;
#ifdef NRDP_HAS_EXECINFO_H
        if (!mSymbols)
            mSymbols = backtrace_symbols(mStack, mSize);
#ifdef NRDP_HAS_CXA_DEMANGLE
        if (!mDemangled[idx + 1] && mSymbols)
            mDemangled[idx + 1] = demangle(mSymbols[idx + 1]);
        if (mDemangled[idx + 1])
            return mDemangled[idx + 1];
#endif
#endif
        return mSymbols ? mSymbols[idx + 1] : 0;
    }
    std::string toString(unsigned maxFrames = 0) const
    {
        std::ostringstream os;
#ifdef NRDP_HAS_EXECINFO_H
        if (mSize) {
            if (!maxFrames) {
                maxFrames = mSize - 1;
            } else {
                maxFrames = std::min(mSize - 1, maxFrames);
            }
            for (unsigned i = 0; i<maxFrames; ++i) {
                if (i)
                    os << "\n";
                os << "[" << std::setfill('0') << std::setw(2) << i + 1 << "/" << mSize - 1 << "] " << frame(i) << " (" << address(i) << ')';
            }
        }
#endif
        return os.str();
    }
private:
    Backtrace()
        : mSize(0), mSymbols(0)
    {
#ifdef NRDP_HAS_CXA_DEMANGLE
        memset(mDemangled, 0, sizeof(mDemangled));
#endif
    }

#ifdef NRDP_HAS_CXA_DEMANGLE
    static inline char *demangle(const char *str)
    {
        if (!str)
            return 0;
        int status;
#ifdef __APPLE__
        char paren[1024];
        sscanf(str, "%*d %*s %*s %s %*s %*d", paren);
#else
        const char *paren = strchr(str, '(');
        if (!paren) {
            paren = str;
        } else {
            ++paren;
        }
#endif
        size_t l;
        if (const char *plus = strchr(paren, '+')) {
            l = plus - paren;
        } else {
            l = strlen(paren);
        }

        char buf[1024];
        size_t len = sizeof(buf);
        if (l >= len)
            return 0;
        memcpy(buf, paren, l + 1);
        buf[l] = '\0';
        char *ret = abi::__cxa_demangle(buf, 0, 0, &status);
        if (status != 0) {
            if (ret)
                free(ret);
#ifdef __APPLE__
            return strdup(paren);
#else
            return 0;
#endif
        }
        return ret;
    }
#endif

    enum { MAX_STACK_SIZE = 50 };
    void *mStack[MAX_STACK_SIZE];
    unsigned mSize;
    mutable char **mSymbols;
#ifdef NRDP_HAS_CXA_DEMANGLE
    mutable char *mDemangled[MAX_STACK_SIZE];
#endif
};
}

#endif
