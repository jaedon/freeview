/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "QATextToSpeechBridge.h"
#include "NrdApplication.h"

using namespace netflix;
using namespace netflix::device;

#define MethodList(Method)                      \
    Method(setPitchRatioHigh)                   \
    Method(setPitchRatioLow)                    \
    Method(setRateRatioHigh)                    \
    Method(setRateRatioLow)                     \
    Method(setVoice)

MethodTable(QATextToSpeechBridge, MethodList)

QATextToSpeechBridge::QATextToSpeechBridge()
    : NfObject("texttospeech", Methods::QATextToSpeechBridge::methods, Methods::QATextToSpeechBridge::MethodCount)
{
    mTextToSpeech = nrdApp()->getDeviceLib()->getTextToSpeech();
}

QATextToSpeechBridge::~QATextToSpeechBridge()
{
}

Variant QATextToSpeechBridge::invoke(int method, const Variant &data)
{
    if (!mTextToSpeech) {
        return Variant();
    }

    //const int id = data.mapValue<int>("id", 0, -1);
    switch (method) {
    case Methods::QATextToSpeechBridge::setPitchRatioHigh:
        mTextToSpeech->setProperty("pitchRatioHigh", data.mapValue("ratio"));
        break;
    case Methods::QATextToSpeechBridge::setPitchRatioLow:
        mTextToSpeech->setProperty("pitchRatioLow", data.mapValue("ratio"));
        break;
    case Methods::QATextToSpeechBridge::setRateRatioHigh:
        mTextToSpeech->setProperty("rateRatioHigh", data.mapValue("ratio"));
        break;
    case Methods::QATextToSpeechBridge::setRateRatioLow:
        mTextToSpeech->setProperty("rateRatioLow", data.mapValue("ratio"));
        break;
    case Methods::QATextToSpeechBridge::setVoice:
        mTextToSpeech->setVoice(data.mapValue<std::string>("voice"));
        break;
    }

    return Variant();
}
