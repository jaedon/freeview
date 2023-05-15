/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ESManagerError.h"

#include <nrdbase/NFErrName.h>
#include <nrdbase/StringFormatter.h>

using namespace std;
using namespace netflix;
using namespace netflix::device;

#define DECLARE_STATIC_STRING(str) \
    static string s_##str = #str

DECLARE_STATIC_STRING(deviceerrorcode);
DECLARE_STATIC_STRING(deviceerrorstring);
DECLARE_STATIC_STRING(errorcode);

//////////
//
// DrmError
//
//////////
ESManagerError::ESManagerError(NFErrorCode errorCode,
                               uint32_t deviceErrorCode,
                               const std::string& deviceErrorString) :
    NFError(errorCode),
    m_deviceErrorCode(deviceErrorCode),
    m_deviceErrorString(deviceErrorString)
{
}

ESManagerError::~ESManagerError()
{
}

//////////
//
// toString
//
//////////
string ESManagerError::toString() const
{
    return StringFormatter::sformat(
        "%s:DeviceSpecificErrorCode=%x:DeviceSpecificErrorString=%s",
        NFErrName::lookup(m_errorCode), m_deviceErrorCode, m_deviceErrorString.c_str());

}

//////////
//
// toVariant
//
//////////
Variant ESManagerError::toVariant() const
{
    Variant map;
    map[s_errorcode] = NFErrName::lookup(m_errorCode);
    map[s_deviceerrorcode] = m_deviceErrorCode;
    map[s_deviceerrorstring] = m_deviceErrorString;
    return map;
}

