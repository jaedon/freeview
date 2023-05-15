/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioMixerOutputALSA.h"
#include <nrddpi/AudioMixerSoftware.h>
#include <nrd/AppLog.h>
#include <nrd/NrdApplication.h>

using namespace netflix;
using namespace netflix::device;

AudioMixerOutputALSA::AudioMixerOutputALSA()
: mHandle(NULL), mRecoverFromUnderflow(true)
{
}

AudioMixerOutputALSA::~AudioMixerOutputALSA()
{
}

void AudioMixerOutputALSA::Recover(const char * s, int err)
{
    if (mRecoverFromUnderflow) {
        err = snd_pcm_recover(mHandle, err, 0);
    }

    if (err < 0) {
        Log::error(TRACE_DPI, "%s (%s)", s, snd_strerror (err));
    } else {
        Log::error(TRACE_DPI, "%s recover from underrun", s);
    }
}

NFErr AudioMixerOutputALSA::Play()
{
    int err;

    if ((err = snd_pcm_start(mHandle)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_start (%s)", snd_strerror (err));
        return NFErr_Bad;
    }
    return NFErr_OK;
}

NFErr AudioMixerOutputALSA::Reset()
{
    /* snd_pcm_drain always fails so this is a workaround. */
    int err;
    snd_pcm_sframes_t delay = 0;

    if ((err = snd_pcm_delay(mHandle, &delay)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_delay (%s)", snd_strerror (err));
    }

    Thread::Sleep(AudioMixerSoftware::framesToMs(delay)+1);

    snd_pcm_close(mHandle);

    mHandle = NULL;

    return NFErr_OK;
}

void AudioMixerOutputALSA::getDeviceInfo(size_t& preferredFrames, size_t& maximumFrames, bool& supportDeferredPlay)
{
    int err;
    snd_pcm_uframes_t periodSize;

    if (!init()) {
        return;
    }

    if ((err = snd_pcm_get_params(mHandle, (snd_pcm_uframes_t*)&maximumFrames, &periodSize)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_get_params (%s)", snd_strerror (err));
    }

    mBuffer.resize(AudioMixerSoftware::framesToBytes(PREFERRED_FRAME_SIZE));
    preferredFrames = PREFERRED_FRAME_SIZE;

    supportDeferredPlay = true;
}

NFErr AudioMixerOutputALSA::getBufferAvailable(size_t& availableFrames)
{
    snd_pcm_sframes_t framesAvailable;

    if (!init()) {
        return NFErr_Bad;
    }

    if ((framesAvailable = snd_pcm_avail(mHandle)) < 0) {
        Recover(__FUNCTION__, framesAvailable);
        return NFErr_Bad;
    }

    availableFrames = framesAvailable;

    return NFErr_OK;
}

NFErr AudioMixerOutputALSA::getBufferDelay(size_t& delayFrames)
{
    int err;
    snd_pcm_sframes_t delay;

    if (!init()) {
        return NFErr_Bad;
    }

    if ((err = snd_pcm_delay(mHandle, &delay)) < 0) {
        Recover(__FUNCTION__, err);
        return NFErr_Bad;
    }

    delayFrames = delay;
    return NFErr_OK;
}

NFErr AudioMixerOutputALSA::getBuffer(void ** buffer_ptr, const size_t requestFrames, size_t& contiguousFrames)
{
    *buffer_ptr = &mBuffer[0];
    contiguousFrames = requestFrames;
    return NFErr_OK;
}

NFErr AudioMixerOutputALSA::commitBuffer(const size_t framesWritten)
{
    int err;
    if ((err = snd_pcm_writei(mHandle, &mBuffer[0], framesWritten)) < 0) {
        Recover(__FUNCTION__, err);
    }

    return NFErr_OK;
}

static void nrdp_snd_lib_error_handler(const char *file, int line, const char *function, int err, const char *fmt, ...)
{
    NRDP_UNUSED(file);
    NRDP_UNUSED(line);
    NRDP_UNUSED(function);
    NRDP_UNUSED(err);

    va_list args;
    va_start(args, fmt);
    NTRACE(TRACE_DPI, "libasound error: %s:%d %s (err: %d): %s",
               file, line, function, err, StringFormatter::vsformat(fmt, args).c_str());
    va_end(args);
}

bool AudioMixerOutputALSA::init(void)
{
    if (!mHandle) {
        int err;
        const char * device = "default";

        snd_lib_error_set_handler(&nrdp_snd_lib_error_handler);

        if ((err = snd_pcm_open(&mHandle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
            Log::error(TRACE_DPI, "snd_pcm_open %s (%s)\n", device, snd_strerror (err));
            return false;
        }

        if ((err = snd_pcm_set_params(mHandle,
                                        SND_PCM_FORMAT_S16_LE,
                                        SND_PCM_ACCESS_RW_INTERLEAVED,
                                        2,      // Channels
                                        48000,  // Sampling rate
                                        0,      // Disallow resampling
                                        AudioMixerSoftware::framesToMs(ALSA_LATENCY_FRAMES)*1000)) < 0) {
            Log::error(TRACE_DPI, "snd_pcm_set_params (%s)\n", snd_strerror (err));
            return false;
        }

        if ((err = snd_pcm_wait(mHandle, 2000)) < 0) {
            Log::error(TRACE_DPI, "snd_pcm_wait (%s)\n", snd_strerror (err));
            return false;
        }
    }
    return true;
}
