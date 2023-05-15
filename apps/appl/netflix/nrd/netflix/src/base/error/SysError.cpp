/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SysError.h"

#include <nrdbase/NFErrName.h>
#include <nrdbase/StringFormatter.h>

using namespace netflix;

// string constants
#define DECLARE_STATIC_STRING(str) \
    static std::string s_##str = #str

DECLARE_STATIC_STRING(errorcode);

const std::string SysError::m_errnoKey("errno");

//////////
//
// SysError
//
//////////
SysError::SysError(long oserr) :
    NFError(NFErr_SysError),
    m_errno(oserr)
{
}

SysError::~SysError()
{
}

//////////
//
// toString
//
//////////
std::string SysError::toString() const
{
    return StringFormatter::sformat(
        "%s:errno=%lld", NFErrName::lookup(m_errorCode), m_errno);
}

//////////
//
// toVariant
//
//////////
Variant SysError::toVariant() const
{
    Variant map;
    map[s_errorcode] = NFErrName::lookup(m_errorCode);
    map[m_errnoKey] = m_errno;
    return map;
}

const std::string& SysError::getErrnoKey()
{
    return m_errnoKey;
}
