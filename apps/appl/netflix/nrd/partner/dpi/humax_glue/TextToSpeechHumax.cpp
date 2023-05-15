/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TextToSpeechHumax.h"
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Thread.h>
#include <nrd/AppThread.h>
#include <nrd/AppLog.h>
#include <list>

using namespace netflix;
using namespace netflix::device;

TextToSpeechHumax::TextToSpeechHumax()
{
}

TextToSpeechHumax::~TextToSpeechHumax()
{
}

void TextToSpeechHumax::setListener(const shared_ptr<Listener>& listener)
{
}

TextToSpeechHumax::Capabilities TextToSpeechHumax::getCapabilities() const
{
    Capabilities caps;
    caps.enabled = false;
    caps.supportsRate = false;
    caps.supportsPitch = false;
    caps.supportsVerbosity = false;
    return caps;
}

//void TextToSpeechHumax::say(unsigned int id, const std::string& text, Pitch /*pitch*/, Rate /*rate*/)
void TextToSpeechHumax::say(unsigned int id, const std::string& text)
{
}

void TextToSpeechHumax::silence(unsigned int id, unsigned int ms)
{
}

void TextToSpeechHumax::stopAndFlush(unsigned int id)
{
}

std::vector<std::string> TextToSpeechHumax::languages() const
{
    return std::vector<std::string>();
}

void TextToSpeechHumax::setLanguage(const std::string& /*language*/)
{
}

std::string TextToSpeechHumax::language() const
{
    return std::string();
}

std::vector<std::string> TextToSpeechHumax::voices() const
{
    return mVoices;
}

void TextToSpeechHumax::setVoice(const std::string& voice)
{
    mVoice = voice;
}

std::string TextToSpeechHumax::voice() const
{
    return mVoice;
}

unsigned int TextToSpeechHumax::rate() const
{
    return 0;
}

unsigned int TextToSpeechHumax::pitch() const
{
    return 0;
}
