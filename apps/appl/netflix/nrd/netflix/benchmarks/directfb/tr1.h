/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TR1_H
#define TR1_H

#include <memory>

/* GCC 4 supplies std::tr1 */
#if __GNUC__ >= 4
#include <tr1/memory>
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::bad_weak_ptr;
using std::tr1::dynamic_pointer_cast;
#else
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
using boost::shared_ptr;
using boost::weak_ptr;
using boost::bad_weak_ptr;
using boost::dynamic_pointer_cast;
#endif

#endif
