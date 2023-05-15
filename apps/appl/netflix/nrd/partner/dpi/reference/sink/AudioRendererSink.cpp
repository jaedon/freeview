/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioRendererSink.h"
#include "../ESPlayerConstants.h"
#include <nrd/AppThread.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Time.h>
#include <nrdbase/Exportable.h>

using namespace netflix::device::esplayer;
using namespace netflix;

class AudioRendererSink::SinkAudioDeviceThread : private Thread
{
public:
    SinkAudioDeviceThread(AudioRendererSink& renderer)
      : Thread(&THREAD_REFERENCE_DPI_DUMMY_AUDIO),
        renderer_(renderer),
        running_(true)
    {
        Start();
        WaitRunning();
    }

    virtual ~SinkAudioDeviceThread()
    {
        running_ = false;
        Wait(); // don't let the thread outlive the object
    }

private:
    virtual void Run()
    {
        static const Time WAIT_ON_SINK_AUDIO_DEV(10);

        while(running_)
        {
            renderer_.audioDeviceTask();
            Thread::Sleep(WAIT_ON_SINK_AUDIO_DEV);
        }
    }

private:
    AudioRendererSink& renderer_;
    bool volatile running_;
};

AudioRendererSink::AudioRendererSink(
    ReferenceClockNative& referenceClock,
    AudioESPlayerNative& ESPlayer,
    uint32_t samplesPerSecond,
    uint32_t outputBufferSamples,
    uint32_t maxFrameSamples, uint32_t maxNumMaxFrames,
    uint32_t timestampQueueSize,
    uint32_t preferredNumOutputChannels)

  : AudioRenderer(referenceClock, ESPlayer)
{
    init(samplesPerSecond, preferredNumOutputChannels, outputBufferSamples,
         maxFrameSamples, maxNumMaxFrames, timestampQueueSize);

    deviceThread_ = new SinkAudioDeviceThread(*this);
}

AudioRendererSink::~AudioRendererSink()
{
    delete deviceThread_;
}

void AudioRendererSink::writeSamples(
    unsigned char* /*dataSeg1*/, uint32_t bytesSeg1,
    unsigned char* /*dataSeg2*/, uint32_t bytesSeg2,
    uint32_t /*bytesSilence*/, void* /*context*/)
{
    consumingSamples_ = (bytesSeg1 > 0 || bytesSeg2 > 0);
}

void AudioRendererSink::audioDeviceTask()
{
    int64_t startTime = Time::mono().val();
    int64_t elapsedTime = 0;
    int64_t numBytesConsumed = 0;
    int64_t targetNumBytesToHaveConsumed = 0;

    uint32_t bytesPerMs = getBytes256thPerTick() / 256;
    int64_t sleepTime = (int64_t)(0.5 + (double)outputBufferSize_ / bytesPerMs);

    while(true)
    {
        // Calculate ms elapsed since we've been running
        elapsedTime = Time::mono().val() - startTime;

        // Calculate num audio bytes consumed in that time
        targetNumBytesToHaveConsumed = elapsedTime * bytesPerMs;

        // If we're due to consume another block of outputBufferSize_ bytes, do so.
        if((targetNumBytesToHaveConsumed-numBytesConsumed) >= outputBufferSize_)
        {
            consumingSamples_ = false;
            play(outputBufferSize_, NULL);
            if(!consumingSamples_)
            {
                return;
            }
            numBytesConsumed += outputBufferSize_;

        } else {

            Thread::Sleep(sleepTime);
        }
    }
}

void AudioRendererSink::resetAudioDevice(uint32_t /*samplesPerSecond*/)
{
}

