/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ASError.h"

#include <nrdbase/NFErrName.h>
#include <nrdbase/StringFormatter.h>

using namespace netflix;

// string constants
#define DECLARE_STATIC_STRING(str)              \
    static std::string s_##str = #str

const std::string ASError::m_asCodeKey("ascode");


DECLARE_STATIC_STRING(errorcode);

//////////
//
// ASError
//
//////////
ASError::ASError(AseErrorCode code) :
    NFError(NFErr_ASError),
    m_asCode(code)
{
}

ASError::~ASError()
{
}

//////////
//
// toString
//
//////////
std::string ASError::toString() const
{
    return StringFormatter::sformat("%s:ascode=%d",
                                    NFErrName::lookup(m_errorCode), m_asCode);
}

//////////
//
// toVariant
//
//////////
Variant ASError::toVariant() const
{
    Variant map;
    map[s_errorcode] = NFErrName::lookup(m_errorCode);
    map[m_asCodeKey] = m_asCode;
    return map;
}
