/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MEDIACONTROL_MC_THREAD_H
#define MEDIACONTROL_MC_THREAD_H

#include <cassert>

#include <nrdbase/ConditionVariable.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Thread.h>

namespace netflix {
namespace mediacontrol {

/**
 * @class McThread McThread.h
 * @brief Media Control Thread Class.
 */
class McThread : public Thread
{
public:
    /**
     * Constructor.
     *
     * @param[in] player the media controller instance.
     * @param[in] priority the priority of thread.
     * @param[in] stackSize size of the stack in words.
     * @param[in] mutexRank the rank of the thread's worker mutex
     * @param[in] description informative description of thread.
     */
    McThread(MutexRank mutexRank, ThreadConfig *config) :
        Thread(config),
        m_workerMutex(mutexRank, config->GetName().c_str()),
        m_running(false)
    {
    }

    virtual ~McThread()
    {
        assert(!m_running);
    }

    virtual void start()
    {
        m_running = true;

        Start();
        WaitRunning();
    }

    virtual void stop()
    {
        m_running = false;

        lock();
        pinch();
        unlock();
    }

    void lock() { m_workerMutex.lock(); }

    void unlock() { m_workerMutex.unlock(); }

    void pinch() { m_workerCondition.signal(); }

    void wait(const Time& timeout)
    {
        m_workerCondition.wait(&m_workerMutex, timeout);
    }

protected:
    size_t m_stackSize;
    ConditionVariable m_workerCondition;
    Mutex m_workerMutex;
    volatile bool m_running;
};

}} // namespace netflix::mediacontrol

#endif // MEDIACONTROL_MC_THREAD_H
