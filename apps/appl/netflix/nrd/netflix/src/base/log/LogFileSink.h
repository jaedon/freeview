/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __LOGFILESINK_H__
#define __LOGFILESINK_H__

#include <assert.h>
#include <sstream>
#include <sys/stat.h>

#include <nrdbase/LexicalCast.h>
#include <nrdbase/Log.h>
#include <nrdbase/LogSink.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Thread.h>

#include <nrdbase/PerformanceMarker.h>

namespace netflix {

class LogFileSink : public LogSink
{
public:
    LogFileSink()
        : mLogFile(0), mFileIndex(0), mMutex(LOGFILE_MUTEX, "LogFile")
    {
    }

    ~LogFileSink()
    {
        if (mLogFile)
            fclose(mLogFile);
    }

    bool open(const std::string &file)
    {
        ScopedMutex lock(mMutex);
        assert(!mLogFile);
        mLogFile = fopen(file.c_str(), "w");
        mFileName = file;
        return mLogFile;
    }

    virtual void receive(const Log::Message &msg)
    {
        PERFORMANCE_MARKER_SCOPED("log.logfilesink.receive");
        assert(msg.m_traceArea);

        if (Configuration::logFileEnabledTracesOnly()
            && msg.m_logLevel == Log::Trace
            && !app()->traceAreas()->isEnabled(msg.m_traceArea)) {
            return;
        }

        const std::string output = ConsoleSink::format(msg, ConsoleSink::defaultFlags() & ~ConsoleSink::LogColor);
        ScopedMutex lock(mMutex);
        if (!mLogFile)
            return;
        if (!fwrite(output.c_str(), output.size(), 1, mLogFile)) {
            fclose(mLogFile);
            mLogFile = 0;
            return;
        }
        fflush(mLogFile);

        off_t size = ftello(mLogFile);

        enum { MaxLogfileSize = 1024 * 1024 * 1024 };

        if (size > MaxLogfileSize) {
            fclose(mLogFile);
            rename(mFileName.c_str(), StringFormatter::sformat("%s.%d", mFileName.c_str(), ++mFileIndex).c_str());
            mLogFile = fopen(mFileName.c_str(), "w");
        }
    }

    std::string fileName() const
    {
        ScopedMutex lock(mMutex);
        return mFileName;
    }

private:
    FILE *mLogFile;
    std::string mFileName;
    uint32_t mFileIndex;
    mutable Mutex mMutex;
};

}

#endif // __LOGFILESINK_H__
