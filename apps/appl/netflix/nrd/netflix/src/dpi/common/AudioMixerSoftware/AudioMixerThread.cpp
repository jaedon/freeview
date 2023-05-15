/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioMixerThread.h"
#include <nrdbase/Thread.h>

using namespace netflix::device;

DEFINE_THREAD(COMMON_DPI_AUDIOMIXER, netflix::Thread::maxPriority, 32768*30);

AudioMixerThread::AudioMixerThread(AudioMixerSoftware& mixer, Task task)
    : Thread(&THREAD_COMMON_DPI_AUDIOMIXER),
      mixer_(mixer),
      task_(task)
{
    Start();
    WaitRunning();
}


AudioMixerThread::~AudioMixerThread()
{
    Wait(); // don't let the thread outlive the object
}

void AudioMixerThread::Run()
{
         (mixer_.*task_)();
}
