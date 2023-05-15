/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NFERRORTIMEDOUT_H__
#define __NFERRORTIMEDOUT_H__

#include <nrdbase/NFError.h>

namespace netflix {

class NFErrorTimedOut : public NFError
{
public:
    NFErrorTimedOut(uint32_t timeoutMS);
    virtual ~NFErrorTimedOut() {}

    virtual std::string toString() const;
    virtual Variant toVariant() const;

    static const std::string& getTimeoutMSKey()
    {
        return m_timeoutMSKey;
    }

private:
    uint32_t m_timeoutMS;
    static const std::string m_timeoutMSKey;
};

} // namespace netflix

#endif // __NFERRORTIMEDOUT_H__
