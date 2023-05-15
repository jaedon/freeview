/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NFERRORSTACK_H__
#define __NFERRORSTACK_H__

#include <vector>
#include <nrdbase/tr1.h>
#include <nrdbase/NFError.h>
#include <nrdbase/Variant.h>

namespace netflix {

//////////
//
// NFErrorStack
//
//////////
class NFErrorStack
{
public:
    inline NFErrorStack() : m_singleton(NFErr_OK) { }
    inline NFErrorStack(NFErrorCode code) : m_singleton(code) { }
    inline NFErrorStack(shared_ptr<NFError> err) : m_singleton(NFErr_OK) { push(err); }

     // ErrorStack will take ownership of ptr's memory
    inline NFErrorStack(NFError *errptr) : m_singleton(NFErr_OK) { push(errptr); }

    inline NFErrorStack(const NFErrorStack &rhs) : m_singleton(rhs.m_singleton),
                                                   m_stack(rhs.m_stack)
    {}
    inline ~NFErrorStack() { }

    // returns true if there is no error
    // i.e. the only error on the stack is NFErr_OK.
    inline bool ok() const { return (m_singleton == NFErr_OK) && (m_stack.empty()); }

    // search the error stack for an NFErrorCode
    bool contains(NFErrorCode code) const;

    // search the error stack for a code, returning the NFError if found
    // return NULL ptr if code not in stack
    shared_ptr<const NFError> find(NFErrorCode code) const;

    // push another error onto the stack
    const NFErrorStack &push(NFErrorCode code);
    const NFErrorStack &push(shared_ptr<NFError> err);
    const NFErrorStack &push(NFError *ptr);  // ErrorStack will take ownership of memory

    // peek at top of stack
    shared_ptr<NFError> peek() const {
        if (m_stack.empty()) // return a constructed error for that singleton code
            return shared_ptr<NFError>(new NFError(m_singleton));
        return m_stack.back();
    }

    inline NFErrorCode peekCode() const {
        if (m_stack.empty())
            return m_singleton;
        return m_stack.back()->code();
    }

    // gets the string representation of the ErrorStack
    std::string toString() const;
    std::string toJsonString() const;
    Variant toVariant() const;

    // comparison operator
    inline bool operator==(const NFErrorStack &rhs) const {
        if (m_stack.size() != rhs.m_stack.size())
            return false;
        if (m_stack.empty())
            return (m_singleton == rhs.m_singleton);
        for (std::size_t i=0; i<m_stack.size(); ++i) {
            if (m_stack[i] != rhs.m_stack[i])
                return false;
        }
        return true;
    }

    inline bool operator==(NFErrorCode rhs) const {
        if (m_stack.empty())
            return (m_singleton == rhs);
        return peekCode() == rhs; // XXX - just check top of stack or whole stack?
    }

    // assignment operator
    inline NFErrorStack &operator=(const NFErrorStack &rhs) {
        if (!(*this == rhs)) {
            m_singleton = rhs.m_singleton;
            m_stack = rhs.m_stack;
        }
        return *this;
    }

private:
    NFErrorCode m_singleton;
    std::vector<shared_ptr<NFError> > m_stack;
};

//////////
//
// other comparison operator's
//
//////////
inline bool operator!=(const NFErrorStack &lhs, const NFErrorStack &rhs)
{
    return !(lhs == rhs);
}

inline bool operator!=(const NFErrorStack &lhs, NFErrorCode rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(NFErrorCode lhs, const NFErrorStack &rhs)
{
    return (rhs == lhs);
}

inline bool operator!=(NFErrorCode lhs, const NFErrorStack &rhs)
{
    return !(rhs == lhs);
}

}

#endif // __NFERRORSTACK_H__
