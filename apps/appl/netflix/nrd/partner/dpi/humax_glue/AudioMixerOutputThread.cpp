#include "AudioMixerOutputBrcm.h"

using namespace netflix::device;

DEFINE_THREAD(DPI_AUDIOMIXER_OUTPUT, 80, 0);

MixerOutputThread::MixerOutputThread(AudioMixerOutputBRCM& aMixer, Task task)
    : Thread(&THREAD_DPI_AUDIOMIXER_OUTPUT), aMixer_(aMixer), task_(task)
{
    Start();
    WaitRunning();
}


MixerOutputThread::~MixerOutputThread()
{
    Wait(); // don't let the thread outlive the object
}

void MixerOutputThread::Run()
{
         (aMixer_.*task_)();
}
