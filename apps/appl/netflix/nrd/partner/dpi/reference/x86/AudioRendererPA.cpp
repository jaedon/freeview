/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioRendererPA.h"
#include <nrdbase/Exportable.h>
#include <nrdbase/Thread.h>

#include <stdio.h>
#include <string.h>

using namespace netflix::device::esplayer;
using namespace netflix;

extern "C" NRDP_EXPORTABLE AudioRenderer *createAudioRenderer(ReferenceClockNative& clock,
                                              AudioESPlayerNative& ESPlayer,
                                              uint32_t samplesPerSecond,
                                              uint32_t outputBufferSamples,
                                              uint32_t maxFrameSamples,
                                              uint32_t maxNumMaxFrames,
                                              uint32_t timestampQueueSize,
                                              uint32_t preferredNumOutputChannels)
{
    return new AudioRendererPA(clock, ESPlayer, samplesPerSecond, outputBufferSamples, maxFrameSamples,
                               maxNumMaxFrames, timestampQueueSize, preferredNumOutputChannels);
}


AudioRendererPA::AudioRendererPA(
    ReferenceClockNative& referenceClock,
    AudioESPlayerNative& ESPlayer,
    uint32_t samplesPerSecond,
    uint32_t outputBufferSamples,
    uint32_t maxFrameSamples, uint32_t maxNumMaxFrames,
    uint32_t timestampQueueSize,
    uint32_t preferredNumOutputChannels)

  : AudioRenderer(referenceClock, ESPlayer),
    isDeviceClosed(true),
    stream_(NULL)
{
    PaError err;

    err = Pa_Initialize();
    if (err != paNoError) {
        Log::error(TRACE_MEDIAPLAYBACK, "Pa_Initialize() error: %s\n", Pa_GetErrorText(err));
        return;
    }

    init(samplesPerSecond, preferredNumOutputChannels, outputBufferSamples,
         maxFrameSamples, maxNumMaxFrames, timestampQueueSize);

    open(samplesPerSecond);
}

AudioRendererPA::~AudioRendererPA()
{
    close();

    PaError err = Pa_Terminate();
    if (err != paNoError) {
        Log::error(TRACE_MEDIAPLAYBACK, "Pa_Terminate() error: %s\n", Pa_GetErrorText(err));
    }
}

void AudioRendererPA::writeSamples(
        unsigned char* dataSeg1, uint32_t bytesSeg1,
        unsigned char* dataSeg2, uint32_t bytesSeg2,
        uint32_t bytesSilence, void* context)
{
    char *data = static_cast<char*>(context);

    if (dataSeg1 != NULL && bytesSeg1 > 0)
        memcpy(data, dataSeg1, bytesSeg1);

    if (dataSeg2 != NULL && bytesSeg2 > 0)
        memcpy(data + bytesSeg1, dataSeg2, bytesSeg2);

    if (bytesSilence)
        memset(data + bytesSeg1 + bytesSeg2, 0, bytesSilence);
}

void AudioRendererPA::resetAudioDevice(uint32_t samplesPerSecond)
{
    close();
    open(samplesPerSecond);
}

void AudioRendererPA::open(uint32_t samplesPerSecond)
{
    PaError err;

    err = Pa_OpenDefaultStream(&stream_,
                               0, /* no input channels */
                               2, /* stereo output */
                               paInt16,
                               samplesPerSecond,
                               paFramesPerBufferUnspecified,
                               &AudioRendererPA::staticCallback,
                               this);
    if (err != paNoError) {
        Log::error(TRACE_MEDIAPLAYBACK, "Pa_OpenDefaultStream() error: %s\n", Pa_GetErrorText(err));
        return;
    }

    err = Pa_SetStreamFinishedCallback(stream_, &AudioRendererPA::staticFinishedCallback);
    if(err != paNoError) {
        return ;
    }

    err = Pa_StartStream(stream_);
    if (err != paNoError) {
        Log::error(TRACE_MEDIAPLAYBACK, "Pa_StartStream() error: %s\n", Pa_GetErrorText(err));
        return;
    }

    isDeviceClosed = false;
}

void AudioRendererPA::close()
{
    PaError err;

    err = Pa_StopStream(stream_);
    if (err != paNoError) {
        Log::error(TRACE_MEDIAPLAYBACK, "Pa_StopStream() error: %s\n", Pa_GetErrorText(err));
        return;
    }

    err = Pa_CloseStream(stream_);
    if (err != paNoError) {
        Log::error(TRACE_MEDIAPLAYBACK, "Pa_OpenDefaultStream() error: %s\n", Pa_GetErrorText(err));
        return;
    }

    isDeviceClosed = true;
}

int AudioRendererPA::callback(const void * /*input*/,
                              void *output,
                              unsigned long frameCount,
                              const PaStreamCallbackTimeInfo * /*timeInfo*/,
                              PaStreamCallbackFlags /*statusFlags*/)
{
    play(frameCount * bytesPerSampleBlock_, output);
    isDeviceClosed = false;
    return 0;
}

int AudioRendererPA::staticCallback(const void *input,
                                    void *output,
                                    unsigned long frameCount,
                                    const PaStreamCallbackTimeInfo *timeInfo,
                                    PaStreamCallbackFlags statusFlags,
                                    void *userData)
{
    return ((AudioRendererPA*)userData)->callback(input, output, frameCount, timeInfo, statusFlags);
}


void AudioRendererPA::staticFinishedCallback(void *userData)
{
    ((AudioRendererPA*)userData)->isDeviceClosed = true;
}

void AudioRendererPA::setFlushState(ESPlayerNative::FlushState flushState)
{
    AudioRenderer::setFlushState(flushState);
    if (isDeviceClosed && flushState == ESPlayerNative::FLUSH_BEGIN) {
        AudioRenderer::setFlushState(ESPlayerNative::FLUSH_END);
    }
}
