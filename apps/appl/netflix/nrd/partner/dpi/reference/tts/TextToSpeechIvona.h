/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEXTTOSPEECH_IVONA
#define TEXTTOSPEECH_IVONA

#include <nrd/ITextToSpeech.h>
#include <ivona_tts.h>

namespace netflix {
namespace device {

class IvonaAudioThread;
class IvonaThread;

class TextToSpeechIvona : public ITextToSpeech
{
public:
    TextToSpeechIvona();
    virtual ~TextToSpeechIvona();

    virtual void setListener(const shared_ptr<Listener>& listener);

    virtual Capabilities getCapabilities() const;

    virtual void say(unsigned int id, const std::string& text);
    virtual void silence(unsigned int id, unsigned int ms);
    virtual void stopAndFlush(unsigned int id);

    virtual std::vector<std::string> languages() const;
    virtual void setLanguage(const std::string& language);
    virtual std::string language() const;

    virtual std::vector<std::string> voices() const;
    virtual void setVoice(const std::string& voice);
    virtual std::string voice() const;

    virtual unsigned int rate() const;
    virtual unsigned int pitch() const;

    virtual Verbosity verbosity() const { return VerbosityLow; }

private:
    IvonaThread* mIvona;
    IvonaAudioThread* mAudio;
    std::vector<std::string> mVoices;
    std::string mVoice;
};

}} // namespace netflix::device

#endif
