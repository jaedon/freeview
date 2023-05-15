/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __COMPILER_H__
#define __COMPILER_H__

/* NRDP_COMPILER_CLANG - Clang  */
#if defined(__clang__)
# define NRDP_COMPILER_CLANG
#endif

/* NRDP_COMPILER_GCC - GNU Compiler Collection */
#if defined(__GNUC__)
# define NRDP_COMPILER_GCC
#endif

/* NRDP_COMPILER_MSVC - MS Visual Studio */
#if defined(_MSC_VER)
# define NRDP_COMPILER_MSVC
#endif

#ifndef NRDP_ALWAYS_INLINE
#if defined(NRDP_COMPILER_GCC) && defined(NDEBUG)
#  define NRDP_ALWAYS_INLINE inline __attribute__((__always_inline__))
#elif defined(NRDP_COMPILER_MSVC)
#  define NRDP_ALWAYS_INLINE __forceinline
#else
#  define NRDP_ALWAYS_INLINE inline
# endif
#endif

#ifndef NRDP_NEVER_INLINE
#if defined(NRDP_COMPILER_GCC)
#  define NRDP_NEVER_INLINE __attribute__((__noinline__))
#else
#  define NRDP_NEVER_INLINE
# endif
#endif

#ifndef NRDP_UNLIKELY
#if defined(NRDP_COMPILER_GCC)
#  define NRDP_UNLIKELY(x) __builtin_expect((x), 0)
#else
#  define NRDP_UNLIKELY(x) (x)
# endif
#endif

#ifndef NRDP_LIKELY
#if defined(NRDP_COMPILER_GCC)
#  define NRDP_LIKELY(x) __builtin_expect((x), 1)
#else
#  define NRDP_LIKELY(x) (x)
# endif
#endif

#ifndef NRDP_NO_RETURN
#if defined(NRDP_COMPILER_GCC)
#  define NRDP_NO_RETURN __attribute((__noreturn__))
#elif defined(NRDP_COMPILER_MSVC)
#  define NO_RETURN __declspec(noreturn)
#else
#  define NRDP_NO_RETURN
# endif
#endif

#ifndef NRDP_WARN_UNUSED_RETURN
#if defined(NRDP_COMPILER_GCC)
#  define NRDP_WARN_UNUSED_RETURN __attribute__ ((warn_unused_result))
#else
#  define NRDP_WARN_UNUSED_RETURN
# endif
#endif

#ifndef NRDP_UNUSED
# define NRDP_UNUSED(x) (void)x
#endif
#ifndef NRDP_UNUSED_RETURN
namespace netflix { inline void __nf_ignore_return_function(int, ...) { } }
# define NRDP_UNUSED_RETURN(x) netflix::__nf_ignore_return_function(0, (x))
#endif

// The generic assertion template:
template <bool assert> struct NRDP_STATIC_ASSERTION;

// Partial specialization for true assertions:
template <> struct NRDP_STATIC_ASSERTION<true> {
    static int const value = 1;
};

// Partial specialization for false assertions is intentionally missing.

// This is done so the line number can become part of the enum name:
#define NRDP_ASSERTION_ENUM_NAME2(x) NRDP_ASSERTION_ENUM_ ## x
#define NRDP_ASSERTION_ENUM_NAME(x) NRDP_ASSERTION_ENUM_NAME2(x)

// The useful assertion macro.  This is the external interface:
#define NRDP_STATIC_ASSERT(x) enum {                                    \
        NRDP_ASSERTION_ENUM_NAME(__LINE__) = NRDP_STATIC_ASSERTION<x>::value \
    }

#endif /* __COMPILER_H__ */
