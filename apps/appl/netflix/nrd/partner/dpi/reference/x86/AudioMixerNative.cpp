/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

// FFMPEG headers
extern "C" {
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include "AudioMixerNative.h"
#include "../RendererManager.h"
#include <nrd/AppLog.h>
#include <nrd/NrdApplication.h>

using namespace netflix;
using namespace netflix::device::esplayer;
using namespace netflix::device;

class PlayTimer;

struct NativeClip {
    NativeClip()
      : listener(NULL)
#if !HAVE_OPENAL
      , stream(NULL)
#endif
    {
    }

    std::vector<unsigned char> data;
    int channels;
    int sample_rate;
    std::string URL;

    IAudioMixer::AudioListener *listener;
    unsigned play_pos;
    double volume;

#if HAVE_OPENAL
    ALuint buffer;
    ALuint source;
    shared_ptr<PlayTimer> timer;
#else
    PaStream *stream;
    bool playing;
#endif
};

void ffmpeg_log_callback_audio_mixer(void* ptr, int level, const char* fmt, va_list vl)
{
    // we don't report sound effect decoding quality
    NRDP_UNUSED(ptr);
    NRDP_UNUSED(level);
    NRDP_UNUSED(fmt);
    NRDP_UNUSED(vl);
}

#if HAVE_OPENAL
class PlayTimer : public Application::Timer
{
public:
    PlayTimer(int interval, NativeClip *clip)
      : Timer(interval)
      , mClip(clip)
    {
    }

    virtual void timerFired()
    {
        NrdApplication::instance()->stopTimer(mClip->timer);
        if (mClip->listener) {
            NTRACE(TRACE_DPI, "%s: calling playDone listener for %s", __FUNCTION__, mClip->URL.c_str());
            mClip->listener->playDone(mClip->URL);
        }
    }

private:
    NativeClip *mClip;
};
#else
//#define PA_DEBUG
#ifdef PA_DEBUG
// portaudio.h doesn't define these things, and the library doesn't export them
// by default, so if you want to use this, go hack portaudio first
extern "C" {
typedef void (*PaUtilLogCallback ) (const char *log);
void PaUtil_SetDebugPrintFunction(PaUtilLogCallback  cb);
}

static void AudioMixerNative_PaDebugPrint(const char *log)
{
    NTRACE(TRACE_DPI, "[PortAudio] %s", log);
}
#else
#define PaUtil_SetDebugPrintFunction(x)
#endif
#endif

AudioMixerNative::AudioMixerNative()
{
    av_log_set_callback(ffmpeg_log_callback_audio_mixer);
    av_register_all();

#if HAVE_OPENAL
    mDevice = alcOpenDevice(NULL);
    NTRACE(TRACE_DPI, "alcOpenDevice: %d", alGetError());
    mContext = alcCreateContext(mDevice, NULL);
    NTRACE(TRACE_DPI, "alcCreateContext: %d", alGetError());
    alcMakeContextCurrent(mContext);
    NTRACE(TRACE_DPI, "alcMakeContextCurrent: %d", alGetError());

    ALfloat listenerPos[]={0.0,0.0,4.0};
    ALfloat listenerVel[]={0.0,0.0,0.0};
    ALfloat listenerOri[]={0.0,0.0,1.0, 0.0,1.0,0.0};

    alListenerfv(AL_POSITION, listenerPos);
    NTRACE(TRACE_DPI, "alListenerfv: %d", alGetError());
    alListenerfv(AL_VELOCITY,listenerVel);
    NTRACE(TRACE_DPI, "alListenerfv: %d", alGetError());
    alListenerfv(AL_ORIENTATION,listenerOri);
    NTRACE(TRACE_DPI, "alListenerfv: %d", alGetError());
#else
    PaUtil_SetDebugPrintFunction(AudioMixerNative_PaDebugPrint);
    Pa_Initialize();
#endif
}

AudioMixerNative::~AudioMixerNative()
{
#if HAVE_OPENAL
    alcMakeContextCurrent(NULL);
    alcDestroyContext(mContext);
    alcCloseDevice(mDevice);
#else
    Pa_Terminate();
#endif
}

IAudioMixer::Capability AudioMixerNative::getCapability() const
{
    IAudioMixer::Capability capability;
    capability.uiSoundBufferSize = 0;
    capability.uiSoundBufferTransferSize = 0;
    return capability;
}

std::vector<AudioCodecType> AudioMixerNative::supportedCodecs()
{
    std::vector<AudioCodecType> types;
    types.push_back(HEAAC_AUDIO);
    types.push_back(OGG_VORBIS_AUDIO);
    types.push_back(WAV_AUDIO);
    types.push_back(MP3_AUDIO);
    types.push_back(WMA_AUDIO);
    return types;
}

NFErr AudioMixerNative::prepareClip(const std::string &URL, const DataBuffer &data,
                                    AudioClip &aClip, unsigned int &clipSize)
{
    NTRACE(TRACE_DPI, "%s: preparing for %s", __FUNCTION__, URL.c_str());

    AVProbeData probe;
    probe.filename = URL.c_str();
    probe.buf_size = data.size();
    probe.buf = (unsigned char *)av_mallocz(probe.buf_size+AVPROBE_PADDING_SIZE);
    memcpy(probe.buf, data.data(), probe.buf_size);

    AVInputFormat * fmt = av_probe_input_format(&probe, 1);
    if (!fmt) {
        return NFErr_BadParameter;
    }

    AVIOContext * pb = avio_alloc_context(probe.buf, probe.buf_size, 0, NULL, NULL, NULL, NULL);

    AVFormatContext * pFormatCtx = avformat_alloc_context();
    pFormatCtx->pb = pb;
    if (avformat_open_input(&pFormatCtx, URL.c_str(), fmt, NULL) != 0) {
        avio_close(pb);
        return NFErr_FileNotOpened;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        avformat_close_input(&pFormatCtx);
        avio_close(pb);
        return NFErr_NotFound;
    }

    AVStream * audioStream = NULL;
    for (unsigned i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO ) {
            audioStream = pFormatCtx->streams[i];
            break;
        }
    }

    if (!audioStream) {
        avformat_close_input(&pFormatCtx);
        avio_close(pb);
        return NFErr_Bad;
    }

    AVCodecContext * aCodecCtx = audioStream->codec;

    AVCodec * aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
    if (!aCodec) {
        avformat_close_input(&pFormatCtx);
        avio_close(pb);
        return NFErr_NotFound;
    }

    avcodec_open2(aCodecCtx, aCodec, NULL);

    NativeClip *clip = new NativeClip;
    clip->URL = URL;
    clip->channels = aCodecCtx->channels;
    clip->sample_rate = aCodecCtx->sample_rate;
    clip->timer.reset(new PlayTimer(0, clip));

    clipSize = 0;
    AVPacket packet;
    av_init_packet(&packet);

    SwrContext * swr = swr_alloc();
    av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr, "in_sample_rate", aCodecCtx->sample_rate, 0);
    av_opt_set_int(swr, "out_sample_rate", aCodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", aCodecCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int(swr, "flags", SWR_FLAG_RESAMPLE, 0);

    if (aCodecCtx->channel_layout == 0) {
        if (aCodecCtx->channels == 2) {
            av_opt_set_int(swr, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
        } else {
            av_opt_set_int(swr, "in_channel_layout", AV_CH_LAYOUT_MONO, 0);
        }
    } else {
        av_opt_set_int(swr, "in_channel_layout", aCodecCtx->channel_layout, 0);
    }

    if (swr_init(swr) != 0 ) {
        return NFErr_Bad;
    }

    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        if (packet.stream_index != audioStream->index) {
            av_free_packet(&packet);
            continue;
        }

        AVFrame * decoded_frame = avcodec_alloc_frame();
        int got_frame_ptr = 0;

        avcodec_decode_audio4(aCodecCtx, decoded_frame, &got_frame_ptr, &packet);
        if (got_frame_ptr > 0) {
            int decodedBytes = av_samples_get_buffer_size(NULL,
                                                          decoded_frame->channels,
                                                          decoded_frame->nb_samples,
                                                          AV_SAMPLE_FMT_S16, 1);
            if (decodedBytes > 0) {
                clip->data.resize(clipSize+decoded_frame->nb_samples * 4);

                uint8_t * dst[SWR_CH_MAX];
                for (int i = 0; i < SWR_CH_MAX; i++) {
                    dst[i] = &clip->data[clipSize];
                }

                swr_convert(swr,
                            dst,
                            decoded_frame->nb_samples,
                            (const uint8_t**)(decoded_frame->extended_data),
                            decoded_frame->nb_samples);

                clipSize += decoded_frame->nb_samples * 4;
                clip->data.resize(clipSize);
            }
        }
        av_frame_free(&decoded_frame);
        av_free_packet(&packet);
    }

    swr_free(&swr);
    avcodec_close(aCodecCtx);
    avformat_close_input(&pFormatCtx);
    avio_close(pb);

#if HAVE_OPENAL
    alGenBuffers(1, &clip->buffer);
    NTRACE(TRACE_DPI, "alGenBuffers: %x", alGetError());

    alBufferData(clip->buffer,
                 AL_FORMAT_STEREO16,
                 &clip->data[0], clip->data.size(), clip->sample_rate);
    NTRACE(TRACE_DPI, "alBufferData: %x", alGetError());

    alGenSources(1, &clip->source);
    NTRACE(TRACE_DPI, "alGenSources: %x", alGetError());
    alSourcef(clip->source, AL_PITCH, 1.0f);
    NTRACE(TRACE_DPI, "alSourcef: %x", alGetError());
    alSourcef(clip->source, AL_GAIN, 1.0f);
    NTRACE(TRACE_DPI, "alSourcef: %x", alGetError());
    ALfloat sourcePos[]={ 0.0, 0.0, 0.0};
    ALfloat sourceVel[]={ 0.0, 0.0, 0.0};
    alSourcefv(clip->source, AL_POSITION, sourcePos);
    NTRACE(TRACE_DPI, "alSourcefv: %x", alGetError());
    alSourcefv(clip->source, AL_VELOCITY, sourceVel);
    NTRACE(TRACE_DPI, "alSourcefv: %x", alGetError());
    alSourcei(clip->source, AL_BUFFER, clip->buffer);
    NTRACE(TRACE_DPI, "alSourcei: %x", alGetError());
    alSourcei(clip->source, AL_LOOPING, AL_FALSE);
    NTRACE(TRACE_DPI, "alSourcei: %x", alGetError());
#endif

    aClip = static_cast<AudioClip>(clip);

    NTRACE(TRACE_DPI, "%s: done preparing for %s", __FUNCTION__, URL.c_str());

    return NFErr_OK;
}

void AudioMixerNative::freeClip(AudioClip aClip, const std::string &URL)
{
    NRDP_UNUSED(URL);
    NTRACE(TRACE_DPI, "%s: freeing %s", __FUNCTION__, URL.c_str());
    NativeClip *clip = static_cast<NativeClip*>(aClip);
#if HAVE_OPENAL
    clip->timer.reset();
    alDeleteSources(1, &clip->source);
    NTRACE(TRACE_DPI, "alDeleteSources: %x", alGetError());
    alDeleteBuffers(1, &clip->buffer);
    NTRACE(TRACE_DPI, "alDeleteBuffers: %x", alGetError());
#else
    if (clip && clip->stream)
        Pa_CloseStream(clip->stream);
#endif
    delete clip;
}

#if !HAVE_OPENAL
static int audio_callback(const void *, void *output,
                          unsigned long frameCount,
                          const PaStreamCallbackTimeInfo*,
                          PaStreamCallbackFlags,
                          void *userData)
{

    uint32_t volumeInPercent = RendererManager::isMuted() ? 0 : RendererManager::getVolume();
    NativeClip *clip = static_cast<NativeClip*>(userData);

    if (!clip->playing)
        return paComplete;
    unsigned num = frameCount * clip->channels;
    unsigned sz = num * sizeof(uint16_t);
    double volume = clip->volume * (static_cast<double> (volumeInPercent)) / 100;

    if (clip->play_pos + sz > clip->data.size())
        sz = clip->data.size() - clip->play_pos;
    memcpy(output, &clip->data[clip->play_pos], sz);
    clip->play_pos += sz;
    if (clip->volume != 1.0f) {
        int16_t *out = static_cast<int16_t *>(output);
        for (unsigned long i = 0; i < num; i++) {
            int32_t val = out[i] * volume;
            if (val > -32768 && val < 32767)
                out[i] = val;
            else if (val < -32768)
                out[i] = -32768;
            else
                out[i] = 32767;
        }
    }
    if (clip->play_pos < clip->data.size())
        return paContinue;
    return paComplete;
}

static void audio_finished(void *userData)
{
    NativeClip *clip = static_cast<NativeClip*>(userData);
    NTRACE(TRACE_DPI, "AudioMixerNative: completed playback of %s", clip->URL.c_str());
    clip->playing = false;
    clip->play_pos = 0;
    clip->volume = 1.0f;
    if (clip->stream) {
        // portaudio are known to have bug regarding termination. Pa_StopStream
        // will cause that bug to appear.
        // Pa_StopStream(clip->stream);
    }
    if (clip->listener) {
        IAudioMixer::AudioListener *l = clip->listener;
        clip->listener = NULL;
        l->playDone(clip->URL);
    }
}
#endif

NFErr AudioMixerNative::playClip(AudioClip aClip, double volume,
                                 AudioListener *listener, const std::string &URL,
                                 PlayId &playId, int, int, Ease, Ease)
{
    NRDP_UNUSED(URL);
    NTRACE(TRACE_DPI, "%s: starting playback of %s", __FUNCTION__, URL.c_str());

    NativeClip *clip = static_cast<NativeClip*>(aClip);
    clip->listener = listener;
    clip->play_pos = 0;
    clip->volume = volume;

#if HAVE_OPENAL
    int bytes_per_sec = 2 * clip->sample_rate * sizeof(uint16_t); // Always converting to stereo
    uint64_t time = ((uint64_t)clip->data.size() * 1000) / bytes_per_sec;
    NTRACE(TRACE_DPI, "%s: %s has %d channels and %d sample_rate so bytes per sec is %d, size is %zu so ms is %lld\n",
           __FUNCTION__, URL.c_str(), clip->channels, clip->sample_rate, bytes_per_sec, clip->data.size(), time);

    clip->timer->restart(time);
    playId = (PlayId)1;

    alSourcef(clip->source, AL_GAIN, clip->volume);
    alSourcePlay(clip->source);
    NTRACE(TRACE_DPI, "alSourcePlay: %x", alGetError());
#else
    PaError err;

    clip->playing = true;
    if (!clip->stream) {
        err = Pa_OpenDefaultStream(&clip->stream,
                                   0, /* no input channels */
                                   clip->channels,
                                   paInt16,
                                   clip->sample_rate,
                                   paFramesPerBufferUnspecified,
                                   audio_callback,
                                   clip);
        if(err != paNoError) {
            return NFErr_Bad;
        }
    }

    err = Pa_SetStreamFinishedCallback(clip->stream, audio_finished);
    if(err != paNoError) {
        return NFErr_Bad;
    }

    err = Pa_StartStream(clip->stream);
    if(err != paNoError) {
        return NFErr_Bad;
    }

    playId = static_cast<PlayId>(clip->stream);
#endif

    NTRACE(TRACE_DPI, "%s: started playback of %s", __FUNCTION__, URL.c_str());

    return NFErr_OK;
}

NFErr AudioMixerNative::stopClip(AudioClip aClip, const std::string &URL, PlayId, int, Ease)
{
    NRDP_UNUSED(URL);
    NTRACE(TRACE_DPI, "%s: stopping playback of %s", __FUNCTION__, URL.c_str());

    NativeClip *clip = static_cast<NativeClip*>(aClip);

#if HAVE_OPENAL
    alSourceStop(clip->source);
    NTRACE(TRACE_DPI, "alSourceStop: %x", alGetError());

    clip->timer->timerFired();
#else
    clip->playing = false;
#endif

    return NFErr_OK;
}

double AudioMixerNative::getVolume()
{
    return 1.0; // TODO: implement
}

NFErr AudioMixerNative::setVolume(double, int, Ease)
{
    return NFErr_NotImplemented;
}
