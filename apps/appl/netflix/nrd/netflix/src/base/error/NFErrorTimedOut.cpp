/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/NFErrorTimedOut.h>

#include <nrdbase/NFErrName.h>
#include <nrdbase/StringFormatter.h>

using namespace netflix;

const std::string NFErrorTimedOut::m_timeoutMSKey("timeoutMS");

//////////
//
// NFErrorTimedOut
//
//////////
NFErrorTimedOut::NFErrorTimedOut(uint32_t timeoutMS)
    : NFError(NFErr_TimedOut),
      m_timeoutMS(timeoutMS)
{
}

//////////
//
// toString
//
//////////
std::string NFErrorTimedOut::toString() const
{
    return StringFormatter::sformat("%s:timeoutMS=%d", NFErrName::lookup(m_errorCode),
                                    m_timeoutMS);
}

//////////
//
// toVariant
//
//////////
Variant NFErrorTimedOut::toVariant() const
{
    Variant map;
    map["errorcode"] = NFErrName::lookup(m_errorCode);
    map[m_timeoutMSKey] = m_timeoutMS;
    return map;
}
