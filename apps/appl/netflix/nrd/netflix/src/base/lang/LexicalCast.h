/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
// The lexical_cast utility provides a generic way to cast between strings and
// numeric types.
#ifndef Netflix_Utilities_LexicalCast_h_
#define Netflix_Utilities_LexicalCast_h_

#include <cassert>
#include <ios>
#include <limits>
#include <locale>
#include <sstream>
#include <string>
#include <nrdbase/Compiler.h>

namespace netflix {

namespace LexicalCastPrivate { // prevent enclosing namespace clutter

// This is the functor that sets up the ios format parameters for the desired
// numeric base.  There is no general implementation of this functor because
// there is no mechanism in <ios> for parsing/generating arbitrary-base values.
// If we ever need that functionality (I should think not), we can add it here.
// For now, if you try to use an unsupported numeric base, you will get a
// compile-time error.
template <int base_> struct setup_ios_format; // intentionally unimplemented

// This is the functor that is used when the base is unspecified.
template <> struct setup_ios_format<0> {
  void operator()(std::ios_base& stream) {
    stream.imbue(std::locale::classic());
    stream.unsetf(std::ios_base::basefield); // use implicit base conversion
  }
};
// Bases specified by lexical_cast template parameter:
template <> struct setup_ios_format<8> {
  void operator()(std::ios_base& stream) {
    stream.imbue(std::locale::classic());
    stream.setf(std::ios_base::oct, std::ios_base::basefield);
  }
};
template <> struct setup_ios_format<10> {
  void operator()(std::ios_base& stream) {
    stream.imbue(std::locale::classic());
    stream.setf(std::ios_base::dec, std::ios_base::basefield);
  }
};
template <> struct setup_ios_format<16> {
  void operator()(std::ios_base& stream) {
    stream.imbue(std::locale::classic());
    stream.setf(std::ios_base::hex, std::ios_base::basefield);
  }
};

// This is the Helper functor declaration.  Specializations of this provide the
// ability for specific types to convert to/from lexicographical
// representations.  A general casting functor is intentionally unavailable
// because if a cast doesn't fall into one of the specializations, lexical_cast
// is most likely the wrong cast to be using.
template <typename target_, int base_, typename source_> struct Helper;

// std::string  -->  arbitrary type
template <typename target_, int base_>
struct Helper<target_, base_, std::string> {
  // Force a compile-time error if base_ is specified for floating point types:
  NRDP_STATIC_ASSERT((
    (base_ == 0) ||
      (!std::numeric_limits<target_>::is_specialized ||
        std::numeric_limits<target_>::is_integer)
  ));
  target_ operator()(std::string const& src) {
    target_ tgt = target_(); // type must be POD or have default constructor
    std::istringstream istr(src);
    setup_ios_format<base_>()(istr);
    istr >> tgt;
    return tgt;
  }
};

// arbitrary type  -->  std::string
template <int base_, typename source_>
struct Helper<std::string, base_, source_> {
  // Force a compile-time error if base_ is specified for floating point types:
  NRDP_STATIC_ASSERT((
    (base_ == 0) ||
      (!std::numeric_limits<source_>::is_specialized ||
        std::numeric_limits<source_>::is_integer)
  ));
  std::string operator()(source_ const& src) {
    std::ostringstream ostr;
    setup_ios_format<base_>()(ostr);
    ostr << src;
    return ostr.str();
  }
};

// The "base_" template parameter is unused for bool specializations because it
// is irrelevant.  Any attempt to specify "base_" will result in a compile-time
// error.

// std::string  -->  bool
template <int base_>
struct Helper<bool, base_, std::string> {
  NRDP_STATIC_ASSERT((base_ == 0)); // compile-time error if base is specified
  bool operator()(std::string const& src) {
    bool value1 = false;
    { // artificial scope -- test with boolalpha set
      std::istringstream istr(src);
      istr.imbue(std::locale::classic());
      istr.setf(std::ios_base::boolalpha);
      istr >> value1;
    }
    bool value2 = false;
    { // artificial scope -- test without boolalpha set
      std::istringstream istr(src);
      istr.imbue(std::locale::classic());
      istr >> value2;
    }
    return value1 || value2;
  }
};

// bool  -->  std::string
template <int base_>
struct Helper<std::string, base_, bool> {
  NRDP_STATIC_ASSERT((base_ == 0)); // compile-time error if base is specified
  std::string operator()(bool const src) {
    std::ostringstream ostr;
    ostr.imbue(std::locale::classic());
    ostr.setf(std::ios_base::boolalpha); // produce "true" or "false"
    ostr << src;
    return ostr.str();
  }
};

// The "base_" template parameter is unused for conversion from one
// lexicographical type to another because it is ambiguous (it doesn't know if
// the specified base applies to the source or destination).  In fact, any
// attempt to specify "base_" in this situation will result in a compile-time
// error.

// std::string  -->  std::string
// This specialization is trivial, but necessary to resolve the compile-time
// ambiguity between arbitrary-->std::string and std::string-->arbitrary.
template <int base_>
struct Helper<std::string, base_, std::string> {
  NRDP_STATIC_ASSERT((base_ == 0)); // compile-time error if base is specified
  std::string operator()(std::string const& src) {
    return src;
  }
};

// char*  -->  std::string
template <int base_>
struct Helper<std::string, base_, char*> {
  NRDP_STATIC_ASSERT((base_ == 0)); // compile-time error if base is specified
  std::string operator()(char const* src) {
    return std::string(src);
  }
};

// char const*  -->  std::string
template <int base_>
struct Helper<std::string, base_, char const*> {
  NRDP_STATIC_ASSERT((base_ == 0)); // compile-time error if base is specified
  std::string operator()(char const* src) {
    return std::string(src);
  }
};

// char*  -->  arbitrary type
template <typename target_, int base_>
struct Helper<target_, base_, char*> {
  target_ operator()(char const* src) {
    return Helper<target_, base_, std::string>()(src);
  }
};

// char const*  -->  arbitrary type
template <typename target_, int base_>
struct Helper<target_, base_, char const*> {
  target_ operator()(char const* src) {
    return Helper<target_, base_, std::string>()(src);
  }
};

// Specifically disallow casting to pointer types:
template <typename target_, int base_, typename source_>
struct Helper<target_*, base_, source_>; // intentionally unimplemented

} // namespace LexicalCastPrivate

// ----------------------------------------------------------------------------
// External Interface ---------------------------------------------------------
// ----------------------------------------------------------------------------
template <typename target_, int base_, typename source_>
target_ lexical_cast(source_ const& src) {
  return LexicalCastPrivate::Helper<target_, base_, source_>()(src);
}

template <typename target_, typename source_>
target_ lexical_cast(source_ const& src) {
  return LexicalCastPrivate::Helper<target_, 0/*unspecified*/, source_>()(src);
}

// Only use if you don't know the base at compile-time:
template <typename target_, typename source_>
target_ lexical_cast(source_ const& src, int const base) {
  switch (base) {
  case 8: return lexical_cast<target_, 8, source_>(src);
  case 10: return lexical_cast<target_, 10, source_>(src);
  case 16: return lexical_cast<target_, 16, source_>(src);
  }
  return lexical_cast<target_, source_>(src); // let the parser figure it out
}

} // namespace netflix

#endif
