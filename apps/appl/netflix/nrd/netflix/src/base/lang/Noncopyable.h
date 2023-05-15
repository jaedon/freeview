/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef Noncopyable_h
#define Noncopyable_h

namespace netflix {
/**
 * @class Noncopyable Noncopyable.h
 * @brief Prevents copy constructor and assignment operator for an
 *        inheriting class.
 */
class Noncopyable
{
protected:
    /** Constructor. */
    Noncopyable() {}

    /** Destructor. */
    ~Noncopyable() {}

private:
    /** Copy constructor private and unimplemented to prevent use. */
    Noncopyable(Noncopyable const&);

    /** Assignment operator private and unimplemented to prevent use. */
    Noncopyable const& operator=(Noncopyable const&);
};
}

#endif
