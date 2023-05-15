/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "PlaybackDeviceError.h"

#include <nrdbase/NFErrName.h>
#include <nrdbase/StringFormatter.h>
#include <sstream>

using namespace netflix;
using namespace netflix::device;

const std::string PlaybackDeviceError::m_iespStatusCodeKey("iespStatusCode");
const std::string PlaybackDeviceError::m_deviceSpecificErrorCodeKey("deviceSpecificErrorCode");
const std::string PlaybackDeviceError::m_deviceSpecificErrorMsgKey("deviceSpecificErrorMsg");

//////////
//
// PlaybackDeviceError
//
//////////
PlaybackDeviceError::PlaybackDeviceError(int32_t param1, ullong param2, const std::string& param3) :
    NFError(NFErr_PlaybackDeviceError),
    m_iespStatusCode(param1),
    m_deviceSpecificErrorMsg(param3)
{
    std::stringstream errcode;

    errcode << std::showbase << std::hex << param2;
    m_deviceSpecificErrorCode = errcode.str();
}

PlaybackDeviceError::~PlaybackDeviceError()
{
}

//////////
//
// toString
//
//////////
std::string PlaybackDeviceError::toString() const
{
    return StringFormatter::sformat("%s:param1=%d,param2=%s,param3=%s",
                                    NFErrName::lookup(m_errorCode),
                                    m_iespStatusCode, m_deviceSpecificErrorCode.c_str(),
                                    m_deviceSpecificErrorMsg.c_str());
}

//////////
//
// toVariant
//
//////////
Variant PlaybackDeviceError::toVariant() const
{
    Variant map;
    map["errorcode"] = NFErrName::lookup(m_errorCode);
    map[m_iespStatusCodeKey] = m_iespStatusCode;
    map[m_deviceSpecificErrorCodeKey] = m_deviceSpecificErrorCode;
    map[m_deviceSpecificErrorMsgKey] = m_deviceSpecificErrorMsg;
    return map;
}

const std::string& PlaybackDeviceError::getIespStatusCodeKey()
{
    return m_iespStatusCodeKey;
}

const std::string& PlaybackDeviceError::getDeviceSpecificErrorCodeKey()
{
    return m_deviceSpecificErrorCodeKey;
}

const std::string& PlaybackDeviceError::getDeviceSpecificErrorMsgKey()
{
    return m_deviceSpecificErrorMsgKey;
}
