/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __AS_ERROR_H__
#define __AS_ERROR_H__

#include <nrdbase/NFError.h>

#include <nrdbase/AseErrorCodes.h>

namespace netflix {

class ASError : public NFError
{
public:
    ASError(AseErrorCode code);
    virtual ~ASError();

    virtual std::string toString() const;
    virtual Variant toVariant() const;

    static std::string getAseErrorCodeKey()
    {
        return m_asCodeKey;
    }

    AseErrorCode code() const { return m_asCode; }

private:
    AseErrorCode m_asCode;
    static const std::string m_asCodeKey;
};

}

#endif // __AS_ERROR_H__
