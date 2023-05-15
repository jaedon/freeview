/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef Endian_h
#define Endian_h

#include <nrdbase/config.h>
#include <nrdbase/Compiler.h>

#include <unistd.h>

// #ifdefery to detect endianness adapted from code taken from http://www.boost.org/doc/libs/1_47_0/boost/detail/endian.hpp
// Copyright notice:

// Copyright 2005 Caleb Epstein
// Copyright 2006 John Maddock
// Copyright 2010 Rene Rivera
// Distributed under the Boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(NRDP_LITTLE_ENDIAN) && !defined(NRDP_BIG_ENDIAN) && !defined(NRDP_PDP_ENDIAN)
#if defined (__GLIBC__)
#  include <endian.h>
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#   define NRDP_LITTLE_ENDIAN
#elif (__BYTE_ORDER == __BIG_ENDIAN)
#   define NRDP_BIG_ENDIAN
#elif (__BYTE_ORDER == __PDP_ENDIAN)
#   define NRDP_PDP_ENDIAN
#else
#   error Unknown machine endianness detected.
#  endif
#elif defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN) || \
    defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__) || \
    defined(_STLP_BIG_ENDIAN) && !defined(_STLP_LITTLE_ENDIAN)
#  define NRDP_BIG_ENDIAN
#elif defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN) || \
    defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__) || \
    defined(_STLP_LITTLE_ENDIAN) && !defined(_STLP_BIG_ENDIAN)
#  define NRDP_LITTLE_ENDIAN
#elif defined(__sparc) || defined(__sparc__) \
   || defined(_POWER) || defined(__powerpc__) \
   || defined(__ppc__) || defined(__hpux) || defined(__hppa) \
   || defined(_MIPSEB) || defined(_POWER) \
   || defined(__s390__)
#  define NRDP_BIG_ENDIAN
#elif defined(__i386__) || defined(__alpha__) \
   || defined(__ia64) || defined(__ia64__) \
   || defined(_M_IX86) || defined(_M_IA64) \
   || defined(_M_ALPHA) || defined(__amd64) \
   || defined(__amd64__) || defined(_M_AMD64) \
   || defined(__x86_64) || defined(__x86_64__) \
   || defined(_M_X64) || defined(__bfin__)
#  define NRDP_LITTLE_ENDIAN
#else
#  error CPU endianness unknown! You need to fix Endian.h
# endif
#endif

#if !defined(NRDP_GPU_LITTLE_ENDIAN) && !defined(NRDP_GPU_BIG_ENDIAN) && !defined(NRDP_GPU_PDP_ENDIAN)
#if defined(NRDP_LITTLE_ENDIAN)
#  define NRDP_GPU_LITTLE_ENDIAN
#elif defined(NRDP_BIG_ENDIAN)
#  define NRDP_GPU_BIG_ENDIAN
#elif defined(NRDP_PDP_ENDIAN)
#  define NRDP_GPU_PDP_ENDIAN
#else
#  error GPU endianness unknown
# endif
#endif

#include <algorithm> // for std::reverse

namespace netflix {
namespace Endian {

inline bool is_little() {
#ifdef __GNUC__
  // The GNU compiler will optimize this union implementation.
  union { int i; char c; } x;
  x.i = 1;
  return x.c;
#else
  // Other compilers will optimize this implementation.
  // GCC 3+ does not optimize this away (see bugzilla #26069).
  unsigned x = 1;
  return *reinterpret_cast<char*>(&x);
#endif
}

inline bool is_big() {
  return !is_little();
}

// The most general byteswap case.
template <std::size_t n_, typename type_>
struct byteswap_helper {
  type_ operator()(type_ val) {
    unsigned char* const v = reinterpret_cast<unsigned char*>(&val);
    std::reverse(v, v + n_);
    return val;
  }
};

// Specializations are for optimization:
template <typename type_>
struct byteswap_helper<1, type_> { // trivial case for single-byte values
  type_ operator()(type_ val) { return val; }
};

// These are the public interface functions:
template <typename type_>
type_ byteswap(type_ val) {
  return byteswap_helper<sizeof(type_), type_>()(val);
}

template <typename type_>
type_ native_to_big(type_ val) {
  return is_big() ? val : byteswap(val);
}
template <typename type_>
type_ native_to_little(type_ val) {
  return is_big() ? byteswap(val) : val;
}
template <typename type_>
type_ big_to_native(type_ val) {
  return is_big() ? val : byteswap(val);
}
template <typename type_>
type_ little_to_native(type_ val) {
  return is_big() ? byteswap(val) : val;
}

// It would be easy to make the endian-ness an enum template parameter here
// instead of defining both "big" and "little" endian classes that are nearly
// identical.  But it would be awkward to use these classes that way.  This is
// a typical case for template typedefs, but until C++0x gives that to us
// (hopefully), it is best to just define these as separate classes.

// These classes are typically used in ways that rely on implementation-
// dependent behavior.  (Think reinterpret_cast and type-punning.)  However,
// all implementations that I'm familiar with will correctly support this
// behavior, so they should be somewhat safe to use.  A static assertion is
// used inside each class to help ensure that these objects will be no larger
// or smaller than the type they represent.  This provides some protection
// against unintentional modifications that could have an effect on the size of
// the representation.  It also can warn us of C++ implementations that don't
// support our usage patterns.

template <typename type_>
class big {
public:
  big(type_ const& v = type_()) : value(Endian::native_to_big(v)) {
    NRDP_STATIC_ASSERT((sizeof *this == sizeof(type_)));
  }
  operator type_() const { return Endian::big_to_native(value); }
private:
  type_ value;
};

template <typename type_>
class little {
public:
  little(type_ const& v = type_()) : value(Endian::native_to_little(v)) {
    NRDP_STATIC_ASSERT((sizeof *this == sizeof(type_)));
  }
  operator type_() const { return Endian::little_to_native(value); }
private:
  type_ value;
};

}} // namespace netflix::Endian

#endif
