/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "CurlError.h"

#include <nrdbase/NFErrName.h>
#include <nrdbase/StringFormatter.h>

using namespace std;
using namespace netflix;

// string constants
#define DECLARE_STATIC_STRING(str) \
    static string s_##str = #str

DECLARE_STATIC_STRING(errorcode);

const std::string CurlError::m_curlECodeKey("ecode");
const std::string CurlError::m_curlMCodeKey("mcode");

//////////
//
// CurlError
//
//////////
CurlError::CurlError(CURLcode code) :
    NFError(NFErr_CurlError),
    m_curlCode(code),
    m_curlMCode(CURLM_OK)
{
}

CurlError::CurlError(CURLMcode mcode) :
    NFError(NFErr_CurlError),
    m_curlCode(CURLE_OK),
    m_curlMCode(mcode)
{
}

CurlError::~CurlError()
{
}

//////////
//
// toString
//
//////////
string CurlError::toString() const
{
    if (m_curlMCode == CURLM_OK)
    {
        return StringFormatter::sformat(
            "%s:ecode=%d", NFErrName::lookup(m_errorCode), m_curlCode);
    }
    else
    {
        return StringFormatter::sformat(
            "%s:mcode=%d", NFErrName::lookup(m_errorCode), m_curlMCode);
    }
}

//////////
//
// toVariant
//
//////////
Variant CurlError::toVariant() const
{
    Variant map;
    map[s_errorcode] = NFErrName::lookup(m_errorCode);
    if (m_curlMCode == CURLM_OK)
    {
        map[m_curlECodeKey] = m_curlCode;
    }
    else
    {
        map[m_curlMCodeKey] = m_curlMCode;
    }
    return map;
}
