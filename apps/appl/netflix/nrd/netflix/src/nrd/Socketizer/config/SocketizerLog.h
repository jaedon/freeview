/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SocketizerLog_h
#define SocketizerLog_h

#include "SocketizerMutex.h"
#include <sstream>
#include <stdio.h>
#include <string>
#include <string.h>

namespace netflix {
namespace socketizer {

class Log
{
public:
    static void enable(bool en);
    static void initialize(FILE* f);

    Log();
    Log(const Log& other);
    ~Log();

    Log& operator=(const Log& other);

    template<typename T>
    Log& operator<<(T t)
    {
        if (mData->hasData)
            mData->data.write(" ", 1);
        else
            mData->hasData = true;
        mData->data.operator<<(t);
        return *this;
    }

private:
    class LogData
    {
    public:
        LogData();
        ~LogData();

        int ref;
        bool hasData;
        std::ostringstream data;
    };

    LogData* mData;
    static FILE* sFile;
    static Mutex sFileMutex;
    static bool sEnabled;

    friend class LogData;
};

inline Log log() { return Log(); }

inline Log::Log()
    : mData(new LogData)
{
}

inline Log::Log(const Log& other)
    : mData(0)
{
    if (other.mData) {
        mData = other.mData;
        ++mData->ref;
    }
}

inline Log::~Log()
{
    if (mData) {
        if (!--mData->ref)
            delete mData;
    }
}

inline Log& Log::operator=(const Log& other)
{
    if (mData) {
        if (!--mData->ref) {
            delete mData;
            mData = 0;
        }
    }
    if (other.mData) {
        mData = other.mData;
        ++mData->ref;
    }
    return *this;
}

inline Log::LogData::LogData()
    : ref(1), hasData(false)
{

}

inline Log::LogData::~LogData()
{
    MutexLocker locker(Log::sFileMutex);
    if (Log::sEnabled)
        fprintf(Log::sFile ? Log::sFile : stderr, "%s\n", data.str().c_str());
}

template<>
inline Log& Log::operator<<(const char* c)
{
    if (mData->hasData)
        mData->data.write(" ", 1);
    else
        mData->hasData = true;
    mData->data.write(c, strlen(c));
    return *this;
}
}
}

#endif
