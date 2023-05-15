/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef DEVICE_MIXERTHREAD_H
#define DEVICE_MIXERTHREAD_H

#include <nrdbase/Thread.h>

DECLARE_THREAD(COMMON_DPI_AUDIOMIXER);

namespace netflix {
namespace device {

class AudioMixerSoftware;

class AudioMixerThread : public Thread
{
public:
    typedef void (AudioMixerSoftware::*Task)();

    /**
     * Constructor.
     *
     * @param[in] device the playback device instance.
     * @param[in] task the task (function) to be executed.
     * @param[in] priority the priority of thread.
     * @param[in] stackSize size of the stack in words.
     * @param[in] description informative description of thread.
     */
    AudioMixerThread(AudioMixerSoftware& mixer, Task task);

    /**
     * Destructor.
     */
    virtual ~AudioMixerThread();

private:
    virtual void Run();
    AudioMixerSoftware& mixer_;
    Task task_;
};

} // namespace device
} // namespace netflix

#endif
