/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __CURL_ERROR_H__
#define __CURL_ERROR_H__

#include <curl/curl.h>
#include <nrdbase/NFError.h>

namespace netflix {

//
// Wrap a curl error code into an NFError
//
class CurlError : public NFError
{
public:
    CurlError(CURLcode code);
    CurlError(CURLMcode mcode);
    virtual ~CurlError();

    virtual std::string toString() const;
    virtual Variant toVariant() const;

    static const std::string& getCurlECodeKey()
    {
        return m_curlECodeKey;
    }
    static const std::string& getCurlMCodeKey()
    {
        return m_curlMCodeKey;
    }

private:
    CURLcode m_curlCode;
    CURLMcode m_curlMCode;

    static const std::string m_curlECodeKey;
    static const std::string m_curlMCodeKey;

};

}

#endif // __CURL_ERROR_H__
