#ifndef AUDIOMIXER_OUTPUT_THREAD_H
#define AUDIOMIXER_OUTPUT_THREAD_H

#include <nrdbase/Thread.h>

DECLARE_THREAD(DPI_AUDIOMIXER_OUTPUT);

namespace netflix {
namespace device {

class AudioMixerOutputBRCM;

/**
 * @class DeviceThread DeviceThread.h
 * @brief Wrapper of any thread that is running on the playback device.
 */
class MixerOutputThread : private Thread
{
public:
    typedef void (AudioMixerOutputBRCM::*Task)();

    /**
     * Constructor.
     *
     * @param[in] audio mixer output handle
     * @param[in] task the task (function) to be executed.
     * @param[in] priority the priority of thread.
     * @param[in] stackSize size of the stack in words.
     * @param[in] description informative description of thread.
     */
    MixerOutputThread(AudioMixerOutputBRCM& aMixer, Task task);

    /**
     * Destructor.
     */
    virtual ~MixerOutputThread();

private:
    virtual void Run();
    AudioMixerOutputBRCM& aMixer_;
    Task task_;
};

} // namespace esplayer
} // namespace device

#endif // AUDIOMIXER_OUTPUT_THREAD_H
