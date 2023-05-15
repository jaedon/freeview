/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/NFErrorStack.h>

#include <nrdbase/JSONFormatter.h>
#include <nrdbase/NFErrName.h>

using namespace netflix;

// string constants
#define DECLARE_STATIC_STRING(str) \
    static std::string s_##str = #str

DECLARE_STATIC_STRING(error);
DECLARE_STATIC_STRING(errorcode);

//////////
//
// contains
//
//////////
bool NFErrorStack::contains(NFErrorCode code) const
{
    if (m_stack.empty())
    {
        return (m_singleton == code);
    }

    // check the stack from the top (back)
    std::vector<shared_ptr<NFError> >::const_reverse_iterator it;
    for (it = m_stack.rbegin(); it != m_stack.rend(); ++it)
    {
        if ((*it)->code() == code)
            return true;
    }

    return false;
}

//////////
//
// find
//
//////////
shared_ptr<const NFError> NFErrorStack::find(NFErrorCode code) const
{
    if (m_stack.empty())
    {
        if (m_singleton == code)
        {
            // return a constructed error for that singleton code
            return shared_ptr<const NFError>(new NFError(m_singleton));
        }
    }
    else
    {
        // check the stack from the top (back)
        std::vector<shared_ptr<NFError> >::const_reverse_iterator it;
        for (it = m_stack.rbegin(); it != m_stack.rend(); ++it)
        {
            if ((*it)->code() == code)
                return *it;
        }
    }

    // not found
    return shared_ptr<const NFError>();
}

//////////
//
// push
//
//////////
const NFErrorStack &NFErrorStack::push(NFErrorCode code)
{
    if (m_stack.empty())
    {
        if (m_singleton != NFErr_OK)
        {
            // push any error code in the singleton onto the stack
            shared_ptr<NFError> singleton(new NFError(m_singleton));
            m_stack.push_back(singleton);
        }
        else
        {
            // we're just replacing a singleton ok with a code
            m_singleton = code;
            return *this;
        }
    }

    shared_ptr<NFError> err(new NFError(code));
    m_stack.push_back(err);

    return *this;
}

const NFErrorStack &NFErrorStack::push(shared_ptr<NFError> err)
{
    if (m_stack.empty() && (m_singleton != NFErr_OK))
    {
        // push any error code in the singleton onto the stack
        shared_ptr<NFError> singleton(new NFError(m_singleton));
        m_stack.push_back(singleton);
    }

    m_stack.push_back(err);

    return *this;
}

const NFErrorStack &NFErrorStack::push(NFError *ptr)
{
    if (m_stack.empty() && (m_singleton != NFErr_OK))
    {
        // push any error code in the singleton onto the stack
        shared_ptr<NFError> singleton(new NFError(m_singleton));
        m_stack.push_back(singleton);
    }

    shared_ptr<NFError> err(ptr);
    m_stack.push_back(err);

    return *this;
}

//////////
//
// toString
//
//////////
std::string NFErrorStack::toString() const
{
    std::string result;

    if (m_stack.empty())
    {
        result = "(" + std::string(NFErrName::lookup(m_singleton)) + ")";
    }
    else
    {
        std::vector<shared_ptr<NFError> >::const_reverse_iterator it;
        for (it = m_stack.rbegin(); it != m_stack.rend(); ++it)
        {
            if (!result.empty())
                result.append(1, ',');

            result += "(" + (*it)->toString() + ")";
        }
    }

    return result;
}

//////////
//
// toJsonString
//
//////////
std::string NFErrorStack::toJsonString() const
{
    return JSONFormatter(JSONFormatter::NullVariantToEmptyString).format(toVariant());
}

//////////
//
// toVariant
//
//////////
Variant NFErrorStack::toVariant() const
{
    Variant array;

    if (m_stack.empty())
    {
        Variant map;
        map[s_errorcode] = NFErrName::lookup(m_singleton);
        array.push_back(map);
    }
    else
    {
        std::vector<shared_ptr<NFError> >::const_reverse_iterator it;
        for (it = m_stack.rbegin(); it != m_stack.rend(); ++it)
        {
            array.push_back((*it)->toVariant());
        }
    }

    return array;
}
