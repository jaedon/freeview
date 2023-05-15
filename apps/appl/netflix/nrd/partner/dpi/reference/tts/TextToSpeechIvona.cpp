/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TextToSpeechIvona.h"
#include <ivona_tts.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Thread.h>
#include <nrd/AppThread.h>
#include <nrd/AppLog.h>
#include <list>
#include <dirent.h>
#include <algorithm>
#include <alsa/asoundlib.h>

static std::vector<std::string> getInstalledVoices(std::string path)
{
    std::vector<std::string> v;
    DIR * p = opendir(path.c_str());

    if (p) {
        dirent * d;
        const std::string prefix = "libvoice_en_us_";

        while ((d = readdir(p)) != NULL) {

            size_t f = std::string(d->d_name).find(prefix);

            if (f != std::string::npos) {
                size_t f2 = std::string(d->d_name).find(".so");
                std::string name = std::string(d->d_name).substr(f+prefix.length(),f2-prefix.length());

                std::vector<std::string>::iterator it = find(v.begin(), v.end(), name);
                if (it == v.end()) {
                    v.push_back(name);
                }
            }
        }
        closedir(p);        
    }

    return v;
}

using namespace netflix;
using namespace netflix::device;

namespace netflix {
namespace device {

struct PcmContainer
{
    std::vector<short> data;
    uint32_t sample_rate;
    uint32_t num_samples() { return data.size(); }
    uint32_t num_channels() { return 1; }
};

class IvonaAudioThread : public Thread
{
public:
    IvonaAudioThread();

    void setListener(const shared_ptr<ITextToSpeech::Listener>& listener);

    void queueAudio(unsigned int id, const PcmContainer& wav);
    void queueAudioFinish(unsigned int id);
    void queueSilence(unsigned int id, unsigned int ms);
    void flush(unsigned int id);

    void stop();

    bool isValid() const;

protected:
    virtual void Run();

private:
    bool initAlsa(int rate, int channels);
    bool deinitAlsa();
    void flushWavs(ScopedMutex& lock);

private:
    mutable Mutex mutex;
    ConditionVariable cond;

    bool stopped;
    bool flushed;
    bool valid;

    struct Entry {
        struct Wave {
            Wave(const PcmContainer& w) : wav(w), ref(1) { }

            PcmContainer wav;
            unsigned int ref;
        };

        Entry()
            : wav(0), ms(0), flush(false), id(0)
        {
        }
        Entry(Wave* w, unsigned int m, bool f, unsigned int i)
            : wav(w), ms(m), flush(f), id(i)
        {
        }
        Entry(const Entry& o)
            : wav(o.wav), ms(o.ms), flush(o.flush), id(o.id)
        {
            if (wav)
                ++wav->ref;
        }
        ~Entry() { if (wav && !--wav->ref) delete wav; }

        Entry& operator=(const Entry& o)
        {
            if (wav && !--wav->ref)
                delete wav;
            wav = o.wav;
            if (wav)
                ++wav->ref;
            ms = o.ms;
            flush = o.flush;
            id = o.id;
            return *this;
        }

        Wave* wav;

        unsigned int ms;
        bool flush;
        unsigned int id;
    };
    std::list<Entry> wavs;
    shared_ptr<ITextToSpeech::Listener> listener;

    snd_pcm_t* alsa;
    snd_pcm_sw_params_t* swparams;

    unsigned int lastId;
};

IvonaAudioThread::IvonaAudioThread()
    : Thread(&THREAD_REFERENCE_DPI_TEXTTOSPEECH_AUDIO),
      mutex(TEXTTOSPEECH_MUTEX, "Ivona audio thread"),
      stopped(false), flushed(false), valid(false),
      lastId(static_cast<unsigned int>(-1))
{
}

bool IvonaAudioThread::isValid() const
{
    ScopedMutex lock(mutex);
    return valid;
}

void IvonaAudioThread::flushWavs(ScopedMutex& lock)
{
    if (!listener) {
        wavs.clear();
        return;
    }
    shared_ptr<ITextToSpeech::Listener> l = listener;

    std::set<unsigned int> utteredIds, flushedIds;

    if (!wavs.empty()) {
        std::list<Entry>::iterator flushEnd = wavs.end();

        std::list<Entry>::iterator it = wavs.begin();
        const std::list<Entry>::const_iterator end = wavs.end();
        while (it != end) {
            if (it->flush) {
                flushedIds.insert(it->id);
                flushEnd = it;
                ++flushEnd;
            } else {
                utteredIds.insert(it->id);
            }
            ++it;
        }

        wavs.erase(wavs.begin(), flushEnd);
    }

    lock.unlock();

    {
        std::set<unsigned int>::const_iterator idit = utteredIds.begin();
        const std::set<unsigned int>::const_iterator idend = utteredIds.end();
        while (idit != idend) {
            l->said(*idit);
            ++idit;
        }
    }
    {
        std::set<unsigned int>::const_iterator idit = flushedIds.begin();
        const std::set<unsigned int>::const_iterator idend = flushedIds.end();
        while (idit != idend) {
            l->stopped(*idit);
            ++idit;
        }
    }
    lock.relock();
}

void IvonaAudioThread::setListener(const shared_ptr<ITextToSpeech::Listener>& l)
{
    listener = l;
}

void IvonaAudioThread::queueAudio(unsigned int id, const PcmContainer& wav)
{
    ScopedMutex lock(mutex);
    if (!valid)
        return;
    wavs.push_back(Entry(new Entry::Wave(wav), 0, false, id));
    cond.signal();
}

void IvonaAudioThread::queueAudioFinish(unsigned int id)
{
    ScopedMutex lock(mutex);
    if (!valid)
        return;
    wavs.push_back(Entry(0, 0, false, id));
    cond.signal();
}

void IvonaAudioThread::queueSilence(unsigned int id, unsigned int ms)
{
    ScopedMutex lock(mutex);
    if (!valid)
        return;
    wavs.push_back(Entry(0, ms, false, id));
    cond.signal();
}

void IvonaAudioThread::flush(unsigned int id)
{
    ScopedMutex lock(mutex);
    if (!valid)
        return;
    wavs.push_back(Entry(0, 0, true, id));
    flushed = true;
    cond.signal();
}

void IvonaAudioThread::stop()
{
    ScopedMutex lock(mutex);
    stopped = true;
    cond.signal();
}

void IvonaAudioThread::Run()
{
    {
        ScopedMutex lock(mutex);
        valid = true;
    }

    for (;;) {
        ScopedMutex lock(mutex);
        while (wavs.empty()) {
            cond.wait(&mutex);
            if (stopped)
                break;
        }
        if (stopped)
            break;
        if (flushed) {
            flushed = false;
            flushWavs(lock);
        }
        if (!wavs.empty()) {
            const Entry& entry = wavs.front();
            const unsigned int id = entry.id;

            if (entry.ms) {
                // sleep for number of ms
                ullong start = Time::mono().ms();
                ullong rem = entry.ms;
                wavs.pop_front();

                shared_ptr<ITextToSpeech::Listener> l = listener;

                lastId = id;
                if (l) {
                    lock.unlock();
                    l->saying(id);
                    lock.relock();
                }

                if (rem > 0) {
                    for (;;) {
                        cond.wait(&mutex, rem);

                        // did we flush or stop the thread? if so, abort
                        if (flushed || stopped) {
                            flushed = false;
                            if (!stopped)
                                flushWavs(lock);
                            break;
                        }

                        const ullong now = Time::mono().ms();
                        if (now - start >= rem)
                            break;
                        rem -= now - start;
                        start = now;
                    }
                }

                if (l) {
                    lock.unlock();
                    l->said(id);
                    lock.relock();
                }

                continue;
            }

            if (!entry.wav) {
                // we're done saying
                wavs.pop_front();

                shared_ptr<ITextToSpeech::Listener> l = listener;
                if (l) {
                    lock.unlock();
                    l->said(id);
                    lock.relock();
                }
                continue;
            }

            const short* buf = &entry.wav->wav.data[0];
            const int len = entry.wav->wav.num_samples();
            int rem = len, pos = 0;

            shared_ptr<ITextToSpeech::Listener> l = listener;

            if (!initAlsa(entry.wav->wav.sample_rate, entry.wav->wav.num_channels())) {
                valid = false;
                return;
            }

            bool lastDiff = false;
            if (lastId != id) {
                lastDiff = true;
                lastId = id;
            }
            lock.unlock();

            if (l && lastDiff)
                l->saying(id);

            int err;
            bool wasFlushed = false;
            snd_pcm_sframes_t frames;

            // playback loop
            for (;;) {
                // did we flush or stop the thread? if so, abort the playback
                lock.relock();
                if (flushed || stopped) {
                    flushed = false;
                    if (!stopped)
                        flushWavs(lock);
                    wasFlushed = true;
                    break;
                }
                lock.unlock();

                // write max 4k at a time
                frames = std::min(4096, rem);

                if ((err = snd_pcm_writei(alsa, buf + pos, frames)) < 0) {
                    Log::error(TRACE_DPI, "Got error %s", snd_strerror(err));
                    if (err == -EAGAIN) {
                        // // wait for device
                        if ((err = snd_pcm_wait(alsa, 1000)) < 0) {
                            Log::error(TRACE_DPI, "snd_pcm_wait (%s)", snd_strerror(err));
                            break;
                        }
                        continue;
                    } else if (err == -EPIPE) {
                        // underflow, try to recover
                        if ((err = snd_pcm_recover(alsa, err, 1)) < 0) {
                            Log::error(TRACE_DPI, "snd_pcm_recover (%s)", snd_strerror(err));
                            break;
                        }
                        continue;
                    } else {
                        Log::error(TRACE_DPI, "snd_pcm_writei (%s)", snd_strerror(err));
                        break;
                    }
                }
                assert(err >= 0);
                frames = err;

                // update
                pos += frames;
                rem -= frames;
                if (rem <= 0) {
                    break;
                }
            }

            if ((err = snd_pcm_drain(alsa)) < 0) {
                Log::error(TRACE_DPI, "snd_pcm_drain (%s)", snd_strerror(err));
                break;
            }

            if (!deinitAlsa()) {
                valid = false;
                return;
            }

            lock.relock();

            if (!wasFlushed)
                wavs.pop_front();

            if (stopped)
                break;
        }
    }

}

bool IvonaAudioThread::initAlsa(int rate, int channels)
{
    swparams = 0;

    int err;
    if ((err = snd_pcm_open(&alsa, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_open (%s)", snd_strerror (err));
        return false;
    }

    if ((err = snd_pcm_set_params(alsa,
                                  SND_PCM_FORMAT_S16_LE,
                                  SND_PCM_ACCESS_RW_INTERLEAVED,
                                  channels, // Channels
                                  rate,     // Sampling rate
                                  1,        // Allow resampling
                                  500000)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_set_params (%s)", snd_strerror(err));
        return false;
    }

    snd_pcm_uframes_t bufferSize, periodSize, boundary;
    if ((err = snd_pcm_get_params(alsa, &bufferSize, &periodSize)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_get_params (%s)", snd_strerror(err));
        return false;
    }

    if ((err = snd_pcm_sw_params_malloc(&swparams)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_sw_params_malloc (%s)", snd_strerror(err));
        return false;
    }
    if ((err = snd_pcm_sw_params_current(alsa, swparams)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_sw_params_current (%s)", snd_strerror(err));
        return false;
    }

    if ((err = snd_pcm_sw_params_get_boundary(swparams, &boundary)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_sw_params_get_boundary (%s)", snd_strerror(err));
        return false;
    }
    if ((err = snd_pcm_sw_params_set_start_threshold(alsa, swparams, (bufferSize / periodSize) * periodSize)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_sw_params_set_start_threshold (%s)", snd_strerror(err));
        return false;
    }
    if ((err = snd_pcm_sw_params_set_avail_min(alsa, swparams, periodSize)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_sw_params_set_avail_min (%s)", snd_strerror(err));
        return false;
    }
    if ((err = snd_pcm_sw_params_set_stop_threshold(alsa, swparams, boundary)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_sw_params_set_stop_threshold (%s)", snd_strerror(err));
        return false;
    }

    if ((err = snd_pcm_sw_params(alsa, swparams)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_sw_params (%s)", snd_strerror(err));
        return false;
    }

    return true;
}

bool IvonaAudioThread::deinitAlsa()
{
    int err;
    if ((err = snd_pcm_close(alsa)) < 0) {
        Log::error(TRACE_DPI, "snd_pcm_close (%s)", snd_strerror(err));
        return false;
    }
    if (swparams) {
        snd_pcm_sw_params_free(swparams);
        swparams = 0;
    }
    return true;
}

class IvonaThread : public Thread
{
public:
    IvonaThread(IvonaAudioThread* audio);

    void stop();
    void say(const std::string& voice, const std::string& text, unsigned int id);
    void silence(unsigned int ms, unsigned int id);
    void flush(unsigned int id);

protected:
    virtual void Run();

private:
    void queueAudio(unsigned int id, const std::string& text);
    void finishAudio(unsigned int id);

private:
    Mutex mutex;
    ConditionVariable cond;

    bool stopped;

    struct Entry {
        unsigned int id;
        unsigned int ms;
        std::string text;
        std::string voice;
        bool flush;
    };

    IvonaAudioThread* audio;

    std::map<std::string, tts_voice *> mVoiceMap;
    tts_streamer *  mS;
    tts_instance *  mTts;

    std::string voice;
    std::list<Entry> entries;
};

IvonaThread::IvonaThread(IvonaAudioThread* a)
    : Thread(&THREAD_REFERENCE_DPI_TEXTTOSPEECH), mutex(TEXTTOSPEECH_MUTEX, "IvonaThread mutex"),
      stopped(false), audio(a)
{
}

void IvonaThread::stop()
{
    ScopedMutex lock(mutex);
    stopped = true;
    cond.signal();
}

void IvonaThread::say(const std::string& v, const std::string& t, unsigned int id)
{
    ScopedMutex lock(mutex);
    const Entry e = { id, 0, t, v, false};
    entries.push_back(e);
    cond.signal();
}

void IvonaThread::flush(unsigned int id)
{
    ScopedMutex lock(mutex);
    const Entry e = { id, 0, std::string(), std::string(), true };
    entries.push_back(e);
    cond.signal();
}

void IvonaThread::silence(unsigned int ms, unsigned int id)
{
    ScopedMutex lock(mutex);
    const Entry e = { id, ms, std::string(), std::string(), false };
    entries.push_back(e);
    cond.signal();
}

void IvonaThread::finishAudio(unsigned int id)
{
    audio->queueAudioFinish(id);
}

void IvonaThread::queueAudio(unsigned int id, const std::string& text)
{
    tts_wave *      w;
    tts_voice *     v = mVoiceMap[voice];

    PcmContainer wav;
    const int samples_per_block = 4096;

    int rate = 27;   // Roughly 300 WPM with "Salli"
    tts_voice_param(v, "set:dur_stretch", &rate);

    int text_length = text.length();
    tts_voice_param(v, "set:max_sentence_length", &text_length);

    if ((mS = tts_streamer_start(v, text.c_str(), -1)) == NULL) {
        Log::error(TRACE_DPI, "Streamer error (%s)", tts_errmsg());
        tts_unload_voice(v);
        tts_destroy(mTts);
    }

    wav.sample_rate = tts_voice_sample_rate(v);

    while ((w = tts_streamer_synth(mS, samples_per_block))) {

        for (uint32_t i = 0; i < w->num_samples; i++) {
            wav.data.push_back(w->samples[i]);
        }

        tts_wave_delete(w);
    }

    if (tts_errno() != TTS_STATUS_OK) {
        Log::error(TRACE_DPI, "Synthesis error (%s)", tts_errmsg());
    }

    tts_streamer_stop(mS);

    audio->queueAudio(id, wav);
}

void IvonaThread::Run()
{
    std::string path = Configuration::dataReadPath() + "/dpi/ivona/";
    std::string pathCert = path + "Certificate_of_authenticity_a9093767_150323085648-signed.ca";

    mTts = tts_create();
    if (mTts == NULL) {
        Log::error(TRACE_DPI, "Failed to create a TTS instance (%s)", tts_errmsg());
        return;
    }

    if (tts_load_certificate_file(mTts, pathCert.c_str()) == NULL) {
        Log::error(TRACE_DPI, "Failed to load the certificate file '%s' (%s)", pathCert.c_str(), tts_errmsg());
        tts_destroy(mTts);
        return;
    }

    std::vector<std::string> s = getInstalledVoices(path);

    for (std::vector<std::string>::const_iterator i = s.begin(); i != s.end(); ++i) {
        tts_voice * v = NULL;
        std::string pathVoiceLib = path + "libvoice_en_us_" + *i + ".so";
        std::string pathVoxDb = path + "vox/vox_en_us_"+ *i +"22i";

        if ((v = tts_load_voice(mTts, pathVoiceLib.c_str(), pathVoxDb.c_str())) != NULL) {
            mVoiceMap[*i] = v;
        } else {
            Log::warn(TRACE_DPI, "Cannot load voice '%s' (%s)", pathVoiceLib.c_str(), tts_errmsg());
        }
    }

    if (mVoiceMap.size() == 0) {
        Log::error(TRACE_DPI, "No voices loaded");
        tts_destroy(mTts);
        return;
    }

    ScopedMutex lock(mutex);
    for (;;) {
        cond.wait(&mutex);
        if (stopped)
            break;
        while (!entries.empty()) {
            const Entry& e = entries.front();
            const unsigned int id = e.id;

            if (e.flush) {
                entries.pop_front();
                lock.unlock();
                audio->flush(id);
                lock.relock();
                continue;
            }

            if (e.ms) {
                const unsigned int ms = e.ms;
                entries.pop_front();
                lock.unlock();
                audio->queueSilence(id, ms);
                lock.relock();
                continue;
            }

            if (e.voice != voice) {
                voice = e.voice;
            }

            std::string text = e.text;
            entries.pop_front();

            // we don't need to hold the mutex for this part
            lock.unlock();

            queueAudio(id, text);

            finishAudio(id);

            lock.relock();
        }
    }

    for (std::vector<std::string>::const_iterator i = s.begin(); i != s.end(); ++i) {
        tts_unload_voice(mVoiceMap[*i]);
    }

    tts_destroy(mTts);

}
}} // namespace netflix::device

TextToSpeechIvona::TextToSpeechIvona()
{
    std::string path = Configuration::dataReadPath() + "/dpi/ivona/";
    mVoices = getInstalledVoices(path);

    if(!mVoices.empty())
    	mVoice = mVoices.back();

    mAudio = new IvonaAudioThread;
    mAudio->Start();

    mIvona = new IvonaThread(mAudio);
    mIvona->Start();
}

TextToSpeechIvona::~TextToSpeechIvona()
{
    mAudio->stop();
    mAudio->Wait();
    delete mAudio;

    mIvona->stop();
    mIvona->Wait();
    delete mIvona;
}

void TextToSpeechIvona::setListener(const shared_ptr<Listener>& listener)
{
    mAudio->setListener(listener);
    ITextToSpeech::setListener(listener);
}

TextToSpeechIvona::Capabilities TextToSpeechIvona::getCapabilities() const
{
    Capabilities caps;
    caps.enabled = true;
    caps.supportsRate = true;
    caps.supportsPitch = true;
    caps.supportsVerbosity = false;
    return caps;
}

void TextToSpeechIvona::say(unsigned int id, const std::string& text)
{
    mIvona->say(mVoice, text, id);
}

void TextToSpeechIvona::silence(unsigned int id, unsigned int ms)
{
    mIvona->silence(ms, id);
}

void TextToSpeechIvona::stopAndFlush(unsigned int id)
{
    mIvona->flush(id);
}

std::vector<std::string> TextToSpeechIvona::languages() const
{
    std::vector<std::string> s;
    s.push_back("en");
    return s;
}

void TextToSpeechIvona::setLanguage(const std::string& /*language*/)
{
}

std::string TextToSpeechIvona::language() const
{
    return "en";
}

std::vector<std::string> TextToSpeechIvona::voices() const
{
    return mVoices;
}

void TextToSpeechIvona::setVoice(const std::string& voice)
{
    mVoice = voice;
}

std::string TextToSpeechIvona::voice() const
{
    return mVoice;
}

unsigned int TextToSpeechIvona::rate() const
{
    return 300;
}

unsigned int TextToSpeechIvona::pitch() const
{
    return 0;
}
