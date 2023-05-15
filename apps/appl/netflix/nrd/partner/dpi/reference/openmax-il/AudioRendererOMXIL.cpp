/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioRendererOMXIL.h"
#include "../ESPlayerConstants.h"
#include <nrd/AppThread.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Time.h>
#include <nrdbase/Exportable.h>

#include <OMX_Broadcom.h>

#include "../DeviceLibNative.h"
#include "../ReferenceClockNative.h"

using namespace netflix;
using namespace netflix::device::esplayer;

class AudioDeviceThreadOMXIL : private Thread
{
public:
    AudioDeviceThreadOMXIL(AudioRendererOMXIL& renderer) : Thread(&THREAD_REFERENCE_DPI_DUMMY_AUDIO), renderer_(renderer), running_(true)
    {
        Start();
        WaitRunning();
    }

    virtual ~AudioDeviceThreadOMXIL()
    {
        running_ = false;
        Wait(); // don't let the thread outlive the object
    }

private:
    virtual void Run()
    {
        static const Time WAIT_ON_OMXIL_AUDIO_DEV(1000);
        while(running_)
            renderer_.audioDeviceTask(WAIT_ON_OMXIL_AUDIO_DEV);
    }

private:
    AudioRendererOMXIL &renderer_;
    bool volatile running_;
};

AudioRendererOMXIL::AudioRendererOMXIL(ReferenceClockNative& referenceClock, AudioESPlayerNative& ESPlayer, uint32_t samplesPerSecond,
                                       uint32_t outputBufferSamples, uint32_t maxFrameSamples, uint32_t maxNumMaxFrames,
                                       uint32_t timestampQueueSize, uint32_t preferredNumOutputChannels) :
                                           AudioRenderer(referenceClock, ESPlayer),
                                           ComponentOMXIL(sConfiguration->omxilAudioRenderer, OMX_IndexParamAudioInit)
{
    AudioRenderer::init(samplesPerSecond, preferredNumOutputChannels, outputBufferSamples,
                        maxFrameSamples, maxNumMaxFrames, timestampQueueSize);

    setState(OMX_StateIdle);
    {
        OMX_PARAM_PORTDEFINITIONTYPE portParam;
        OMX_INIT_STRUCTURE(portParam);
        portParam.nPortIndex = inputPort();
        OMX_VERIFY(OMX_GetParameter(handle(), OMX_IndexParamPortDefinition, &portParam));
        portParam.nPortIndex = inputPort();
        portParam.nBufferSize = 50 * 1024;
        portParam.nBufferCountActual = 1;
        portParam.format.audio.eEncoding = OMX_AUDIO_CodingPCM;
        OMX_VERIFY(OMX_SetParameter(handle(), OMX_IndexParamPortDefinition, &portParam));
    }
    {
        OMX_AUDIO_PARAM_PORTFORMATTYPE format;
        OMX_INIT_STRUCTURE(format);
        format.nPortIndex = inputPort();
        format.eEncoding = OMX_AUDIO_CodingPCM;
        OMX_VERIFY(OMX_SetParameter(handle(), OMX_IndexParamAudioPortFormat, &format));
    }
    {
        OMX_CONFIG_BRCMAUDIODESTINATIONTYPE audioDest;
        OMX_INIT_STRUCTURE(audioDest);
        strncpy((char*)audioDest.sName, "hdmi", 4);
        OMX_VERIFY(OMX_SetParameter(handle(), OMX_IndexConfigBrcmAudioDestination, &audioDest));
    }

    initInputBuffers();
    if(setState(OMX_StateExecuting))
        waitForCommandComplete(OMX_CommandStateSet, OMX_StateExecuting);

    mRenderer.init(sConfiguration->omxilAudioRenderer, OMX_IndexParamAudioInit);
    mRenderer.setState(OMX_StateIdle);
    if(shared_ptr<ComponentOMXIL> clock = getClock())
        mRenderer.receive(clock.get(), clock->inputPort(), mRenderer.inputPort()+1);
    mRenderer.receive(this);
    mRenderer.setState(OMX_StateExecuting);

    mThread = new AudioDeviceThreadOMXIL(*this);
}

AudioRendererOMXIL::~AudioRendererOMXIL()
{
    reset();
    delete mThread;
}

void AudioRendererOMXIL::writeSamples(unsigned char *dataSeg1, uint32_t bytesSeg1,
                                      unsigned char *dataSeg2, uint32_t bytesSeg2,
                                      uint32_t bytesSilence, void *context)
{
    if(OMX_BUFFERHEADERTYPE *buffer = (OMX_BUFFERHEADERTYPE*)context) {
        if(bytesSeg1 || bytesSeg2) {
            static bool started = false;
            if(!started) {
                started = true;
                Log::error(TRACE_LOG, "[%s:%d]: %lld", __FILE__, __LINE__, Time::mono().ms());
            }
        }
        buffer->nFlags = OMX_BUFFERFLAG_ENDOFFRAME;
        buffer->nOffset = 0;
        buffer->nTimeStamp = convertTime(Time::mono());
        buffer->nFilledLen = 0;
        if(bytesSeg1) {
            memcpy(buffer->pBuffer+buffer->nFilledLen, dataSeg1, bytesSeg1);
            buffer->nFilledLen += bytesSeg1;
        }
        if(bytesSeg2) {
            memcpy(buffer->pBuffer+buffer->nFilledLen, dataSeg2, bytesSeg2);
            buffer->nFilledLen += bytesSeg2;
        }
        if(bytesSilence) {
            memset(buffer->pBuffer+buffer->nFilledLen, 0, bytesSilence);
            buffer->nFilledLen += bytesSilence;
        }
        emptyBuffer(buffer);
    }
}

void AudioRendererOMXIL::audioDeviceTask(const Time &wait)
{
    if(OMX_BUFFERHEADERTYPE *buffer = inputBuffer(wait))
        play(buffer->nAllocLen, buffer);
}

void AudioRendererOMXIL::resetAudioDevice(uint32_t /*samplesPerSecond*/)
{
    reset();
}

void AudioRendererOMXIL::reset()
{
    Log::error(TRACE_LOG, "OMX::AudioRenderer RESET");
    sendData(0, 0, OMX_BUFFERFLAG_TIME_UNKNOWN | OMX_BUFFERFLAG_EOS, convertTime(Time(0)), "audiodecoder::reset");
    flushInput();
    flushOutput();
    mRenderer.flushOutput();
    mRenderer.flushInput();
}
