/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "VoiceBridge.h"

#include <nrd/ISystem.h>

using namespace std;
using namespace netflix;

#define MethodList(Method)                      \
    Method(setVoiceGrammar)                     \
    Method(supportsVoice)

MethodTable(VoiceBridge, MethodList);

VoiceBridge::VoiceBridge()
    : NfObject("input", Methods::VoiceBridge::methods, Methods::VoiceBridge::MethodCount),
      mSupportsVoice(false)
{
}

VoiceBridge::~VoiceBridge()
{
}


bool VoiceBridge::invoke(int method, const Variant &var)
{
    bool ok;

    switch (method) {
    case Methods::VoiceBridge::supportsVoice:
        NTRACE(TRACE_NBP,"Methods::VoiceBridge::supportsVoice '%d'\n", var.mapValue<bool>("value", &ok));
        break;
    case Methods::VoiceBridge::setVoiceGrammar:
        NTRACE(TRACE_NBP,"Methods::VoiceBridge::setVoiceGrammar '%s'\n", var.mapValue<string>("grammar", &ok).c_str());
        {
            Variant map;
            Variant alts;
            alts.push_back("bye");
            alts.push_back("fly");
            map["type"] = "voice";
            map["match"] = "dry";
            map["paramAlternatives"] = alts;
            sendEvent("voice", map);
        }
        {
            Variant map;
            Variant alts;
            alts.push_back("stop");
            map["type"] = "voice";
            map["match"] = "top";
            map["paramAlternatives"] = alts;
            sendEvent("voice", map);
        }
        break;
    default:
        return false;
    }
    return true;
}

