/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef _InstrumentationThread_h
#define _InstrumentationThread_h

#include <nrdbase/Noncopyable.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Configuration.h>

namespace netflix {
namespace nrdlog {

class InstrumentationThread : private Noncopyable
{
public:

    static void start();
    static void shutdown();
    static bool switchedAlwaysOn();

    static ConfigurationOptions defaultOptions();

private:

    InstrumentationThread();
    ~InstrumentationThread();

    static InstrumentationThread * sInstance;

    std::auto_ptr<Thread> mThread;
};

}} // netflix::nrdlog


#endif // _InstrumentationThread_h
