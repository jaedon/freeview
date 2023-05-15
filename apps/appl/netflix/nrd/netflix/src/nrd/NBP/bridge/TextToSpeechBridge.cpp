/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TextToSpeechBridge.h"
#include "NrdApplication.h"

using namespace netflix;
using namespace netflix::device;

#define TextToSpeechPropertyList(Property)      \
    Property(capabilities, NfObject::ReadOnly)  \
    Property(language, NfObject::ReadWrite)     \
    Property(languages, NfObject::ReadOnly)     \
    Property(pitch, NfObject::ReadOnly)         \
    Property(rate, NfObject::ReadOnly)          \
    Property(verbosity, NfObject::ReadOnly)     \
    Property(voice, NfObject::ReadOnly)         \
    Property(voices, NfObject::ReadOnly)

PropertyTable(TextToSpeechBridge, TextToSpeechPropertyList, properties)

#define MethodList(Method)                      \
    Method(say)                                 \
    Method(silence)                             \
    Method(stopAndFlush)

MethodTable(TextToSpeechBridge, MethodList)

void TextToSpeechBridge::Listener::saying(unsigned int handle)
{
    Variant data;
    data["id"] = handle;
    bridge->sendEvent("saying", data);
}

void TextToSpeechBridge::Listener::said(unsigned int handle)
{
    Variant data;
    data["id"] = handle;
    bridge->sendEvent("said", data);
}

void TextToSpeechBridge::Listener::stopped(unsigned int handle)
{
    Variant data;
    data["id"] = handle;
    bridge->sendEvent("stopped", data);
}

void TextToSpeechBridge::Listener::capabilitiesChanged()
{
    bridge->propertyUpdated(Properties::TextToSpeechBridge::capabilities);
}

void TextToSpeechBridge::Listener::verbosityChanged()
{
    bridge->propertyUpdated(Properties::TextToSpeechBridge::verbosity);
}

void TextToSpeechBridge::Listener::propertiesChanged()
{
    bridge->propertyUpdated(Properties::TextToSpeechBridge::pitch);
    bridge->propertyUpdated(Properties::TextToSpeechBridge::rate);
}

TextToSpeechBridge::TextToSpeechBridge()
    : NfObject("texttospeech", Properties::TextToSpeechBridge::properties, Properties::TextToSpeechBridge::PropertyCount,
               Methods::TextToSpeechBridge::methods, Methods::TextToSpeechBridge::MethodCount),
      mListener()
{
    mListener.reset(new Listener(this));
    mTextToSpeech = nrdApp()->getDeviceLib()->getTextToSpeech();
    if (mTextToSpeech)
        mTextToSpeech->setListener(mListener);
}

TextToSpeechBridge::~TextToSpeechBridge()
{
}

bool TextToSpeechBridge::getProperty(int index, Variant *result) const
{
    if (!mTextToSpeech) {
        *result = Variant();
        return true;
    }

    switch (index) {
    case Properties::TextToSpeechBridge::capabilities: {
        const ITextToSpeech::Capabilities& c = mTextToSpeech->getCapabilities();
        *result = Variant();
        (*result)["enabled"] = c.enabled;
        (*result)["supportsRate"] = c.supportsRate;
        (*result)["supportsPitch"] = c.supportsPitch;
        (*result)["supportsVerbosity"] = c.supportsVerbosity;
        break; }
    case Properties::TextToSpeechBridge::voice:
        *result = mTextToSpeech->voice();
        break;
    case Properties::TextToSpeechBridge::pitch:
        *result = mTextToSpeech->pitch();
        break;
    case Properties::TextToSpeechBridge::rate:
        *result = mTextToSpeech->rate();
        break;
    case Properties::TextToSpeechBridge::verbosity:
        *result = static_cast<int>(mTextToSpeech->verbosity());
        break;
    case Properties::TextToSpeechBridge::voices: {
        Variant arr(Variant::Type_Array);
        const std::vector<std::string>& voices = mTextToSpeech->voices();
        for (size_t i = 0; i < voices.size(); ++i) {
            arr.push_back(voices[i]);
        }
        *result = arr;
        break; }
    case Properties::TextToSpeechBridge::language:
        *result = mTextToSpeech->language();
        break;
    case Properties::TextToSpeechBridge::languages: {
        Variant arr(Variant::Type_Array);
        const std::vector<std::string>& languages = mTextToSpeech->languages();
        for (size_t i = 0; i < languages.size(); ++i) {
            arr.push_back(languages[i]);
        }
        *result = arr;
        break; }
    default:
        return false;
    }
    return true;
}

bool TextToSpeechBridge::setProperty(int index, const Variant &value)
{
    if (!mTextToSpeech) {
        return true;
    }

    switch (index) {
    case Properties::TextToSpeechBridge::language:
        mTextToSpeech->setLanguage(value.string());
        propertyUpdated(Properties::TextToSpeechBridge::language);
        break;
    }

    return true;
}

Variant TextToSpeechBridge::invoke(int method, const Variant &data)
{
    if (!mTextToSpeech) {
        return Variant();
    }

    const int id = data.mapValue<int>("id", 0, -1);
    switch (method) {
    case Methods::TextToSpeechBridge::say: {
        mTextToSpeech->say(id, data["text"].string());
        break; }
    case Methods::TextToSpeechBridge::silence: {
        mTextToSpeech->silence(id, data.mapValue<int>("ms"));
        break; }
    case Methods::TextToSpeechBridge::stopAndFlush: {
        mTextToSpeech->stopAndFlush(id);
        break; }
    }

    return Variant();
}
