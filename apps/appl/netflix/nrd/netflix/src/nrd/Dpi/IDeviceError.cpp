/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "IDeviceError.h"

#include <nrdbase/NFErrName.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/Backtrace.h>

#include <sstream>

using namespace netflix;
using namespace netflix::device;

// "iespStatusCode" should stay intact since this key is used by DSE
const std::string IDeviceError::m_categoryCodeKey("iespStatusCode");
const std::string IDeviceError::m_deviceSpecificErrorCodeKey("deviceSpecificErrorCode");
const std::string IDeviceError::m_deviceSpecificErrorInfoKey("deviceSpecificErrorInfo");

// optional field to device specific error information
const std::string IDeviceError::m_deviceStackTraceKey("stackTrace");

//////////
//
// IDeviceError
//
//////////
IDeviceError::IDeviceError(esplayer::Status param1,
                           ullong param2,
                           const Variant& param3,
                           bool param4) :
  NFError(NFErr_PlaybackDeviceError),
  m_categoryCode(param1),
  m_deviceSpecificErrorCode(param2),
  m_deviceSpecificErrorInfo(param3)
{
    if(param4){
        m_deviceSpecificErrorInfo[getDeviceStackTraceKey()] = Backtrace::backtrace()->toString();
    }

#ifdef BUILD_DEBUG
    if (param1 == 0){
        // param1 is iesp error status code of OK.
        // Why create error when status is OK!
        assert(0);
    }
    if (param2 == 0) {
        // param2 is device specific error status code.
        // 0x0 for device specific error code does not sound right
        assert(0);
    }
    if (param3.size() < 1) {
        // param3 is device specific message for this error situation.
        // You should provide at least one errorDescription of this error
        assert(0);
    }
#endif
}

IDeviceError::~IDeviceError()
{
}

//////////
//
// toString
//
//////////
std::string IDeviceError::toString() const
{
    return StringFormatter::sformat("%s:param1=%d,param2=0x%llx,param3=%s",
                                    NFErrName::lookup(m_errorCode),
                                    m_categoryCode,
                                    m_deviceSpecificErrorCode,
                                    m_deviceSpecificErrorInfo.toJSON().c_str());
}

//////////
//
// toVariant
//
//////////
Variant IDeviceError::toVariant() const
{
    Variant map;
    map["errorcode"] = NFErrName::lookup(m_errorCode);
    map[m_categoryCodeKey] = m_categoryCode;
    map[m_deviceSpecificErrorCodeKey] = m_deviceSpecificErrorCode;
    map[m_deviceSpecificErrorInfoKey] = m_deviceSpecificErrorInfo;
    return map;
}

esplayer::Status IDeviceError::getCategoryCode()
{
    return m_categoryCode;
}

ullong IDeviceError::getDeviceSpecificErrorCode()
{
    return m_deviceSpecificErrorCode;
}

std::string IDeviceError::getDeviceSpecificErrorCodeStr()
{
    std::stringstream errcode;
    errcode << std::showbase << std::hex << m_deviceSpecificErrorCode;
    return errcode.str();
}

Variant& IDeviceError::getDeviceSpecificErrorInfo()
{
    return m_deviceSpecificErrorInfo;
}

std::string IDeviceError::getDeviceSpecificErrorInfoStr()
{
    return m_deviceSpecificErrorInfo.toJSON();
}

const std::string& IDeviceError::getCategoryCodeKey()
{
    return m_categoryCodeKey;
}

const std::string& IDeviceError::getDeviceSpecificErrorCodeKey()
{
    return m_deviceSpecificErrorCodeKey;
}

const std::string& IDeviceError::getDeviceStackTraceKey()
{
    return m_deviceStackTraceKey;
}

IDeviceError& IDeviceError::operator=(const IDeviceError &rhs)
{
    if( !(*this == rhs) ) {
        m_categoryCode = rhs.m_categoryCode;
        m_deviceSpecificErrorCode = rhs.m_deviceSpecificErrorCode;
        m_deviceSpecificErrorInfo = rhs.m_deviceSpecificErrorInfo;
    }
    return *this;
}
