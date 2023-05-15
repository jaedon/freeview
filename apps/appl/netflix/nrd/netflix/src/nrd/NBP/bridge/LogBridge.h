/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef LogBridge_h
#define LogBridge_h

#include <list>
#include <set>

#include <nrd/config.h>
#include <nrd/NfObject.h>
#include <nrdbase/LogSink.h>
#include <nrdbase/TraceAreas.h>
#include <nrdbase/Time.h>

namespace netflix {

class LogBridge : public NfObject, public LogSink
{
public:
    LogBridge();
    ~LogBridge();

    void shutdown();
    virtual unsigned requestFlags(const Request &) const;
    virtual bool getProperty(int index, Variant *value) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual Variant invoke(int method, const Variant &data);

    // Log::Sink
    virtual void receive(const Log::Message &msg);
private:
    const TraceArea *ensureTraceArea(std::string name, std::string groups);
    void popLogMessages(Variant & array);
    void getCriticalMessages(Variant & array);
    void deleteCriticalMessages(Variant & array);

    typedef std::map<size_t, Log::Message> CriticalMap;

    void loadCriticalErrors();
    static void storeCriticalErrors(const CriticalMap & errors);

    mutable Mutex mMutex;
    std::string mUIContext;
    std::list <Log::Message> mLogmsgs;

    size_t mNextCriticalId;
    CriticalMap mCriticalMap;

    std::set<shared_ptr<TraceArea> > mDynamicTraceAreas;
    int mLogLevel;
    bool mWarned;
};

}

#endif
