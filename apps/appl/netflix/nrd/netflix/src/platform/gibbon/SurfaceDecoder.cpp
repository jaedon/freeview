/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SurfaceDecoder.h"
#include "Animation.h"

#include <string.h>
#include <unistd.h>
#include <map>
#include <algorithm>

#include "SurfaceCache.h"
#include "Screen.h"

#include <nrd/AppThread.h>
#include <nrd/AppLog.h>

#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/Thread.h>

using namespace netflix;
using namespace netflix::gibbon;

NRDP_DEFINE_OBJECTCOUNT(GibbonSurfaceDecodersTimer);

static Mutex sSurfaceDecodersMutex(GIBBON_SURFACEDECODER_THREAD_MUTEX, "GibbonSurfaceDecoderThread");
static SurfaceDecoders::SharedPtr sSurfaceDecodersInstance;

class SurfaceDecodersThread : public Thread
{
public:
    SurfaceDecodersThread();
    void stop();
    void add(int ms, const shared_ptr<SurfaceDecoders::Timer> &timer);
protected:
    virtual void Run();
private:
    std::multimap<Time, shared_ptr<SurfaceDecoders::Timer> > mTimers;
    bool mStopped;
    ConditionVariable mCondition;
    friend class SurfaceDecoders::Timer;

    friend class SurfaceDecoders;
};

SurfaceDecoders::SurfaceDecoders() : mThread(0)
{
    mThread = new SurfaceDecodersThread;
    mThread->Start();
}

SurfaceDecoders::~SurfaceDecoders()
{
    if(mThread) {
        mThread->stop();
        mThread->Wait();
        delete mThread;
    }
}

SurfaceDecoders::SharedPtr SurfaceDecoders::instance()
{
    ScopedMutex _lock(sSurfaceDecodersMutex);
    if(!sSurfaceDecodersInstance && !nrdApp()->hasQuit())
        sSurfaceDecodersInstance.reset(new SurfaceDecoders);
    return sSurfaceDecodersInstance;
}

void SurfaceDecoders::cleanup()
{
    SurfaceDecoders::SharedPtr decoders;
    {
        ScopedMutex _lock(sSurfaceDecodersMutex);
        std::swap(decoders, sSurfaceDecodersInstance);
    }
}

SurfaceDecoder::SharedPtr SurfaceDecoders::decoder(const Resource::SharedPtr &resource) const
{
#if defined(GIBBON_SURFACEDECODER_PLATFORM)
    if(GibbonConfiguration::platformSurfaceDecoder()) {
        if(SurfaceDecoder::SharedPtr d = SurfaceDecoderPlatform::decoder(resource))
            return d;
    }
#endif
#ifdef GIBBON_SURFACEDECODER_JPEG
    if(SurfaceDecoder::SharedPtr d = SurfaceDecoderJPEG::decoder(resource))
        return d;
#endif
#ifdef GIBBON_SURFACEDECODER_PNG
    if(SurfaceDecoder::SharedPtr d = SurfaceDecoderPNG::decoder(resource))
        return d;
#endif
#ifdef GIBBON_SURFACEDECODER_MNG
    if(SurfaceDecoder::SharedPtr d = SurfaceDecoderMNG::decoder(resource))
        return d;
#endif
#ifdef GIBBON_SURFACEDECODER_WEBP
    if(SurfaceDecoder::SharedPtr d = SurfaceDecoderWebP::decoder(resource))
        return d;
#endif
    return SurfaceDecoder::SharedPtr();
}

std::vector<std::string> SurfaceDecoders::formats() const
{
    std::vector<std::string> formats;
#ifdef GIBBON_SURFACEDECODER_JPEG
    formats.push_back(SurfaceDecoderJPEG::extension());
#endif
#ifdef GIBBON_SURFACEDECODER_PNG
    formats.push_back(SurfaceDecoderPNG::extension());
#endif
#ifdef GIBBON_SURFACEDECODER_MNG
    formats.push_back(SurfaceDecoderMNG::extension());
#endif
#ifdef GIBBON_SURFACEDECODER_WEBP
    formats.push_back(SurfaceDecoderWebP::extension());
#endif
    return formats;
}

Variant SurfaceDecoders::libraryInfo() const
{
    Variant result;
#ifdef GIBBON_SURFACEDECODER_JPEG
    result.push_back(SurfaceDecoderJPEG::libraryInfo());
#endif
#ifdef GIBBON_SURFACEDECODER_PNG
    result.push_back(SurfaceDecoderPNG::libraryInfo());
#endif
#ifdef GIBBON_SURFACEDECODER_MNG
    result.push_back(SurfaceDecoderMNG::libraryInfo());
#endif
#ifdef GIBBON_SURFACEDECODER_WEBP
    result.push_back(SurfaceDecoderWebP::libraryInfo());
#endif
    return result;
}

void SurfaceDecoders::dump()
{
    int count = 0;
#if defined(GIBBON_SURFACEDECODER_PLATFORM)
    if(GibbonConfiguration::platformSurfaceDecoder())
        Log::warn(TRACE_LOG, " [%d] Platform", count++);
#endif
    {
        std::vector<std::string> decoders = formats();
        for(std::vector<std::string>::const_iterator it = decoders.begin(); it != decoders.end(); ++it)
            Log::warn(TRACE_LOG, " [%d] %s", count++, it->c_str());
    }
}

DataBuffer SurfaceDecoders::encode(const Surface::SharedPtr &surface) const
{
#ifdef GIBBON_SURFACEDECODER_PNG
    return SurfaceDecoderPNG::encode(surface);
#else
    return DataBuffer();
#endif
}

bool SurfaceDecoders::encode(const Surface::SharedPtr &surface, const std::string &file) const
{
#ifdef GIBBON_SURFACEDECODER_PNG
    const DataBuffer data = SurfaceDecoderPNG::encode(surface);
    const std::string extension = SurfaceDecoderPNG::extension();

#else
    const DataBuffer data;
    const std::string extension;
#endif
    if(!data.empty()) {
        if(FILE *f = fopen((file+"."+extension).c_str(), "w")) {
            NRDP_UNUSED_RETURN(fwrite(data.constData(), data.length(), 1, f));
            fclose(f);
            return true;
        }
    }
    return false;
}

void SurfaceDecoders::setTimeout(int ms, const shared_ptr<Timer> &timer)
{
    mThread->add(ms, timer);
}

void SurfaceDecoders::Timer::stop()
{
    ScopedMutex lock(sSurfaceDecodersMutex);
    mStopped = true;
}

SurfaceDecodersThread::SurfaceDecodersThread() : Thread(&THREAD_GIBBON_SURFACE_CONTROLLER), mStopped(false)
{
}

void SurfaceDecodersThread::add(int ms, const shared_ptr<SurfaceDecoders::Timer> &timer)
{
    ScopedMutex lock(sSurfaceDecodersMutex);
    std::multimap<Time, shared_ptr<SurfaceDecoders::Timer> >::iterator it;
    it = mTimers.insert(std::make_pair(Time::mono() + Time::fromMS(ms), timer));
    if (it == mTimers.begin())
        mCondition.signal();
}

void SurfaceDecodersThread::stop()
{
    {
        ScopedMutex lock(sSurfaceDecodersMutex);
        mStopped = true;
        mCondition.signal();
    }
    while(Wait() != NFErr_OK);
}

void SurfaceDecodersThread::Run()
{
    while (true) {
        PERFORMANCE_MARKER_SCOPED("surfacedecoders.loop");

        shared_ptr<SurfaceDecoders::Timer> timer;
        {
            PERFORMANCE_MARKER_SCOPED("surfacedecoders.loop.idle");

            ScopedMutex lock(sSurfaceDecodersMutex);
            if (mStopped)
                break;

            Time time;
            bool wait = true;
            if (!mTimers.empty()) {
                time = mTimers.begin()->first - Time::mono();
                wait = time.ms();
            }

            if (!wait || mCondition.wait(&sSurfaceDecodersMutex, time) == netflix::NFErr_TimedOut) {
                assert(!mTimers.empty());
                time = mTimers.begin()->first;
                timer = mTimers.begin()->second;
                if(timer->isStopped()) { //timer is done
                    mTimers.erase(mTimers.begin());
                    timer.reset();
                } else if(time > Time::mono()) { //not yet
                    timer.reset();
                } else { //fire
                    mTimers.erase(mTimers.begin());
                }
            }
            if (mStopped)
                break;
        }
        if (timer) {
            PERFORMANCE_MARKER_SCOPED("surfacedecoders.loop.exec");
            timer->execute();
        }
    }
}


