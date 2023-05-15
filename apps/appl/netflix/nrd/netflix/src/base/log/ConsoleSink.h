/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __CONSOLESINK_H__
#define __CONSOLESINK_H__

#include <nrdbase/LogSink.h>
#include <nrdbase/TraceAreas.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Configuration.h>

namespace netflix {

class ConsoleSink : public LogSink
{
public:
    typedef shared_ptr<ConsoleSink> SharedPtr;

    ConsoleSink();

    inline std::list<Log::Message> milestones() const {
        ScopedMutex lock(mMutex);
        return mMilestones;
    }
    inline void shutdownMilestones() {
        ScopedMutex lock(mMutex);
        mMilestones.resize(0);
    }

    virtual void receive(const Log::Message &msg);

    int logLevel() const;
    void setLogLevel(int logLevel);

    enum {
        LogColor        = 0x01,
        LogAbsoluteTime = 0x02,
        LogProcess      = 0x04
    };
    static uint8_t defaultFlags() {
        uint8_t flags = 0;
        if(Configuration::consoleSinkLogColor())
            flags |= LogColor;
        if(Configuration::consoleSinkLogAbsolute())
            flags |= LogAbsoluteTime;
        if(Configuration::consoleSinkLogProcess())
            flags |= LogProcess;
        return flags;
    }
    inline uint8_t flags() const { return mFlags; }
    inline void setFlag(uint8_t flag) { mFlags |= flag; }
    inline void setFlags(uint8_t flags) { mFlags = flags; }

    static std::string format(const Log::Message &msg, uint8_t flags, const Configuration::ColorScheme &colorScheme = Configuration::ColorScheme());
    static std::string format(const Log::Message &msg, const Configuration::ColorScheme &colorScheme = Configuration::ColorScheme()) {
        return format(msg, defaultFlags(), colorScheme);
    }
protected:
    virtual void receive(int logLevel, const std::string &msg);

private:
    std::list<Log::Message> mMilestones;
    mutable Mutex mMutex;
    uint8_t mFlags;
    int mLogLevel;
};

}

#endif // __CONSOLESINK_H__
