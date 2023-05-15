/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TextToSpeechOSX.h"
#import <AppKit/AppKit.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ConditionVariable.h>
#include <nrd/AppThread.h>
#include <nrd/AppLog.h>
#include <list>

class ScopedPool
{
public:
    ScopedPool() { mPool = [[NSAutoreleasePool alloc] init]; }
    ~ScopedPool() { [mPool drain]; }

private:
    NSAutoreleasePool* mPool;
};

class TextToSpeechOSXPrivate;

@interface TTSDelegate : NSObject <NSSpeechSynthesizerDelegate>
{
@public
    netflix::Mutex* mutex;

    TextToSpeechOSXPrivate* priv;
    unsigned int current;
    shared_ptr<netflix::device::ITextToSpeech::Listener> listener;
};

@end

using namespace netflix;
using namespace netflix::device;

class TextToSpeechOSXPrivate : public Thread
{
public:
    TextToSpeechOSXPrivate();
    ~TextToSpeechOSXPrivate();

    void stop();
    void flush(unsigned int id);
    void queueAudio(unsigned int id, const std::string& text);
    void queueSilence(unsigned int id, unsigned int ms);

    std::string voice() const;
    void setVoice(const std::string& voice);

    void setProperty(const std::string& key, const Variant& value);

    float pitch() const;
    float rate() const;

    void setListener(const shared_ptr<ITextToSpeech::Listener>& listener);

    void stoppedSpeaking();

protected:
    virtual void Run();

private:
    void flushEntries(ScopedMutex& lock);

private:
    mutable Mutex mutex;
    ConditionVariable cond;

    NSSpeechSynthesizer* synth;
    TTSDelegate* delegate;
    bool speaking, flushed, stopped;

    enum Flags {
        Flush = 0x1,
        Voice = 0x2,
        Silence = 0x4
    };

    struct Entry
    {
        unsigned int id;
        unsigned int flags;
        unsigned int integer;
        std::string str;
        float flt;
    };
    std::list<Entry> entries;
    std::string currentVoice;

    shared_ptr<netflix::device::ITextToSpeech::Listener> listener;
};

@implementation TTSDelegate

-(void)speechSynthesizer:(NSSpeechSynthesizer *)sender didFinishSpeaking:(BOOL)finishedSpeaking {
    (void)sender;
    (void)finishedSpeaking;

    shared_ptr<netflix::device::ITextToSpeech::Listener> l;
    unsigned int id = 0;
    {
        ScopedMutex lock(*mutex);
        l = listener;
        id = current;
    }

    if (l)
        l->said(id);
    priv->stoppedSpeaking();
}

@end

TextToSpeechOSXPrivate::TextToSpeechOSXPrivate()
    : Thread(&THREAD_REFERENCE_DPI_TEXTTOSPEECH), mutex(TEXTTOSPEECH_MUTEX, "TextToSpeechOSX"),
      speaking(false), flushed(false), stopped(false)
{
    delegate = [[TTSDelegate alloc] init];
    delegate->priv = this;
    delegate->mutex = new Mutex(TEXTTOSPEECH_MUTEX, "TextToSpeechOSX delegate mutex");
}

TextToSpeechOSXPrivate::~TextToSpeechOSXPrivate()
{
    delete delegate->mutex;
    [delegate release];
}

void TextToSpeechOSXPrivate::setProperty(const std::string&, const Variant&)
{
}

std::string TextToSpeechOSXPrivate::voice() const
{
    ScopedMutex lock(mutex);
    return currentVoice;
}

void TextToSpeechOSXPrivate::setVoice(const std::string& v)
{
    ScopedMutex lock(mutex);
    currentVoice = v;
    const Entry e = { 0, Voice, 0, v, 0 };
    entries.push_back(e);
    cond.signal();
}

float TextToSpeechOSXPrivate::pitch() const
{
    ScopedMutex lock(mutex);
    return 0;
}

float TextToSpeechOSXPrivate::rate() const
{
    ScopedMutex lock(mutex);
    return 300;
}

void TextToSpeechOSXPrivate::setListener(const shared_ptr<ITextToSpeech::Listener>& l)
{
    {
        ScopedMutex lock(*delegate->mutex);
        delegate->listener = l;
    }
    ScopedMutex lock(mutex);
    listener = l;
}

void TextToSpeechOSXPrivate::stoppedSpeaking()
{
    ScopedMutex lock(mutex);
    speaking = false;
    cond.signal();
}

void TextToSpeechOSXPrivate::flush(unsigned int id)
{
    ScopedMutex lock(mutex);
    const Entry e = { id, Flush, 0, std::string(), 0 };
    entries.push_back(e);
    flushed = true;
    cond.signal();
}

void TextToSpeechOSXPrivate::stop()
{
    ScopedMutex lock(mutex);
    stopped = true;
    cond.signal();
}

void TextToSpeechOSXPrivate::flushEntries(ScopedMutex& lock)
{
    shared_ptr<ITextToSpeech::Listener> l = listener;

    std::string voice;
    std::vector<unsigned int> utteredIds, flushedIds;

    if (!entries.empty()) {
        std::list<Entry>::iterator flushEnd = entries.end();

        std::list<Entry>::iterator it = entries.begin();
        const std::list<Entry>::const_iterator end = entries.end();
        while (it != end) {
            if (it->flags & Flush) {
                flushedIds.push_back(it->id);
                flushEnd = it;
                ++flushEnd;
            } else if (it->flags & Voice) {
                voice = it->str;
            } else {
                utteredIds.push_back(it->id);
            }
            ++it;
        }

        entries.erase(entries.begin(), flushEnd);
    }

    lock.unlock();

    if (l) {
        {
            std::vector<unsigned int>::const_iterator idit = utteredIds.begin();
            const std::vector<unsigned int>::const_iterator idend = utteredIds.end();
            while (idit != idend) {
                l->said(*idit);
                ++idit;
            }
        }
        {
            std::vector<unsigned int>::const_iterator idit = flushedIds.begin();
            const std::vector<unsigned int>::const_iterator idend = flushedIds.end();
            while (idit != idend) {
                l->stopped(*idit);
                ++idit;
            }
        }
    }
    lock.relock();

    if (!voice.empty()) {
        ScopedPool pool;
        NSString* str = [NSString stringWithUTF8String:voice.c_str()];
        [synth setVoice:str];

        if (l) {
            lock.unlock();
            l->propertiesChanged();
            lock.relock();
        }
    }
}

void TextToSpeechOSXPrivate::Run()
{
    synth = [[NSSpeechSynthesizer alloc] init];
    {
        ScopedMutex lock(*delegate->mutex);
        delegate->listener = listener;
    }
    [synth setDelegate:delegate];

    {
        ScopedMutex lock(mutex);
        currentVoice = [[synth voice] UTF8String];
    }

    for (;;) {
        ScopedMutex lock(mutex);
        if (stopped)
            break;
        if (flushed) {
            if (speaking) {
                speaking = false;
                [synth stopSpeaking];
            }
            flushed = false;
            flushEntries(lock);
        }
        while (speaking || entries.empty()) {
            cond.wait(&mutex);
            if (stopped || flushed)
                break;
        }
        if (stopped)
            break;
        if (flushed) {
            if (speaking) {
                speaking = false;
                [synth stopSpeaking];
            }
            flushed = false;
            flushEntries(lock);
        }
        if (!entries.empty()) {
            assert(!speaking);

            ScopedPool pool;
            const Entry& e = entries.front();
            const unsigned int id = e.id;

            if (e.flags & Voice) {
                NSString* str = [NSString stringWithUTF8String:e.str.c_str()];
                [synth setVoice:str];

                shared_ptr<ITextToSpeech::Listener> l = listener;
                if (l) {
                    lock.unlock();
                    l->propertiesChanged();
                    lock.relock();
                }
            } else if (e.flags & Silence) {
                // sleep for 'ms'
                ullong rem = e.integer;

                shared_ptr<ITextToSpeech::Listener> l = listener;

                if (l) {
                    lock.unlock();
                    l->saying(id);
                    lock.relock();
                }

                if (rem > 0) {
                    ullong start = Time::mono().ms();
                    for (;;) {
                        cond.wait(&mutex, rem);

                        // did we flush or stop the thread? if so, abort
                        if (flushed || stopped) {
                            break;
                        }

                        const ullong now = Time::mono().ms();
                        if (now - start >= rem) {
                            break;
                        }
                        rem -= now - start;
                        start = now;
                    }

                    if (flushed || stopped) {
                        entries.pop_front();
                        continue;
                    }
                }

                if (l) {
                    lock.unlock();
                    l->said(id);
                    lock.relock();
                }
            } else if (!(e.flags & Flush)) {
                assert(!e.flags);
//                    [synth setObject:[NSNumber numberWithFloat:0] forProperty:NSSpeechPitchBaseProperty error:nil];

                shared_ptr<ITextToSpeech::Listener> l = listener;

                NSString* str = [NSString stringWithUTF8String:e.str.c_str()];
                speaking = true;
                lock.unlock();

                {
                    ScopedMutex lock(*delegate->mutex);
                    delegate->current = id;
                }

                [synth startSpeakingString:str];

                if (l)
                    l->saying(id);

                lock.relock();
            }
            entries.pop_front();
        }
    }

    [synth release];
}

void TextToSpeechOSXPrivate::queueAudio(unsigned int id, const std::string& text)
{
    ScopedMutex lock(mutex);
    const Entry e = { id, 0, 0, text, 0 };
    entries.push_back(e);
    cond.signal();
}

void TextToSpeechOSXPrivate::queueSilence(unsigned int id, unsigned int ms)
{
    ScopedMutex lock(mutex);
    const Entry e = { id, Silence, ms, std::string(), 0 };
    entries.push_back(e);
    cond.signal();
}

TextToSpeechOSX::TextToSpeechOSX()
    : priv(new TextToSpeechOSXPrivate)
{
    priv->Start();
}

TextToSpeechOSX::~TextToSpeechOSX()
{
    priv->stop();
    priv->Wait();
    delete priv;
}

void TextToSpeechOSX::setListener(const shared_ptr<Listener>& listener)
{
    priv->setListener(listener);
    ITextToSpeech::setListener(listener);
}

TextToSpeechOSX::Capabilities TextToSpeechOSX::getCapabilities() const
{
    Capabilities caps;
    caps.enabled = true;
    caps.supportsRate = true;
    caps.supportsPitch = true;
    caps.supportsVerbosity = false;
    return caps;
}

void TextToSpeechOSX::say(unsigned int id, const std::string& text)
{
    priv->queueAudio(id, text);
}

void TextToSpeechOSX::silence(unsigned int id, unsigned int ms)
{
    priv->queueSilence(id, ms);
}

void TextToSpeechOSX::stopAndFlush(unsigned int id)
{
    priv->flush(id);
}


std::vector<std::string> TextToSpeechOSX::languages() const
{
    return std::vector<std::string>();
}

void TextToSpeechOSX::setLanguage(const std::string& /*language*/)
{
}

std::string TextToSpeechOSX::language() const
{
    return std::string();
}


std::vector<std::string> TextToSpeechOSX::voices() const
{
    std::vector<std::string> ret;
    {
        ScopedPool pool;
        NSArray* array = [NSSpeechSynthesizer availableVoices];
        for (NSString *string in array) {
            ret.push_back([string UTF8String]);
        }
    }
    return ret;
}

void TextToSpeechOSX::setVoice(const std::string& voice)
{
    ScopedPool pool;
    priv->setVoice(voice);
}

void TextToSpeechOSX::setProperty(const std::string& key, const Variant& value)
{
    priv->setProperty(key, value);
}

std::string TextToSpeechOSX::voice() const
{
    return priv->voice();
}

unsigned int TextToSpeechOSX::rate() const
{
    return priv->rate();
}

unsigned int TextToSpeechOSX::pitch() const
{
    return priv->pitch();
}
