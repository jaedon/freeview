/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DrmError.h"

#include <nrdbase/NFErrName.h>
#include <nrdbase/StringFormatter.h>

using namespace std;
using namespace netflix;
using namespace netflix::device;

#define DECLARE_STATIC_STRING(str) \
    static string s_##str = #str

DECLARE_STATIC_STRING(errorcode);

const string DrmError::m_drmErrorCodeKey("drmcode");

//////////
//
// DrmError
//
//////////
DrmError::DrmError(uint32_t drmCode) :
    NFError(NFErr_DRMFailed),
    m_drmCode(drmCode)
{
}

DrmError::~DrmError()
{
}

//////////
//
// toString
//
//////////
string DrmError::toString() const
{
    return StringFormatter::sformat(
        "%s:DRM_RESULT=%x", NFErrName::lookup(m_errorCode), m_drmCode);
}

//////////
//
// toVariant
//
//////////
Variant DrmError::toVariant() const
{
    Variant map;
    map[s_errorcode] = NFErrName::lookup(m_errorCode);
    map[m_drmErrorCodeKey] = m_drmCode;
    return map;
}

const std::string DrmError::getDrmErrorCodeKey()
{
    return m_drmErrorCodeKey;
}
