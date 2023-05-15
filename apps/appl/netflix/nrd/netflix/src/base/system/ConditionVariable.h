
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef ConditionVariable_h
#define ConditionVariable_h

#include <nrdbase/Noncopyable.h>

#include <nrdbase/Mutex.h>
#include <nrdbase/Time.h>
#include <nrdbase/NFErr.h>

namespace netflix {

class ConditionVariable : private Noncopyable
{
public:
    ConditionVariable();
    ~ConditionVariable();

    void signal();
    void broadcast();

    //time = 0 means wait without timeout
    netflix::NFErr wait(Mutex *mutex, const Time &time = Time(0));
    netflix::NFErr wait(Mutex *mutex, Time *time);

private:
    pthread_cond_t mCond;
};

} // namespace netflix

#endif
