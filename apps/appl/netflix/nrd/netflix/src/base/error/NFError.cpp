/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/NFError.h>

#include <nrdbase/JSONFormatter.h>
#include <nrdbase/NFErrName.h>

using namespace netflix;

// string constants
#define DECLARE_STATIC_STRING(str) \
    static std::string s_##str = #str

DECLARE_STATIC_STRING(errorcode);

//////////
//
// toString
//
//////////
std::string NFError::toString() const
{
    return NFErrName::lookup(m_errorCode);
}

//////////
//
// toVariant
//
//////////
Variant NFError::toVariant() const
{
    Variant map;
    map[s_errorcode] = NFErrName::lookup(m_errorCode);
    return map;
}

//////////
//
// toJsonString
//
//////////
std::string NFError::toJsonString() const
{
    return JSONFormatter(JSONFormatter::NullVariantToEmptyString).format(toVariant());
}

