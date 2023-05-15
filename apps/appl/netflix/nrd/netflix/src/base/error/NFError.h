/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NFERROR_H__
#define __NFERROR_H__

#include <string>
#include <nrdbase/NFErrorCode.h>
#include <nrdbase/Variant.h>

namespace netflix {

//////////
//
// NFError
//
// Subclass this if you want an error with subcodes
//
//////////
class NFError
{
public:
    inline NFError(NFErrorCode code) : m_errorCode(code) { }
    virtual ~NFError() { }

    // returns a string representation of this error
    virtual std::string toString() const;
    virtual Variant toVariant() const;
    std::string toJsonString() const;

    // returns the primary error code
    inline NFErrorCode code() const { return m_errorCode; }

    inline bool operator==(const NFError &rhs) const
    {
        return m_errorCode == rhs.m_errorCode;
    }

    inline bool operator==(NFErrorCode rhs) const
    {
        return m_errorCode == rhs;
    }

protected:
    NFErrorCode m_errorCode;
};

//////////
//
// comparison operators
//
//////////
inline bool operator!=(const NFError &lhs, const NFError &rhs)
{
    return !(lhs == rhs);
}

inline bool operator!=(const NFError &lhs, NFErrorCode rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(NFErrorCode lhs, const NFError &rhs)
{
    return (rhs == lhs);
}

inline bool operator!=(NFErrorCode lhs, const NFError &rhs)
{
    return !(rhs == lhs);
}

}

#endif // __NFERROR_H__
