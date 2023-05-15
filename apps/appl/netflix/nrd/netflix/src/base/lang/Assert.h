/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef Assert_H
#define Assert_H

#include <stdio.h>
#include <stdlib.h>

#if defined(BUILD_DEBUG)
#  define ENABLE_ASSERTS 1
#endif

// By default, ASSERT will abort.
#define ASSERTS_ABORT 1

#define LINE_STRING LINE_CALL(__LINE__)
#define LINE_CALL(x) LINE_LITERAL(x)
#define LINE_LITERAL(x) #x

/**
 * Output a message if condition is false. Do not abort.
 *
 * @param[in] condition the condition to check.
 * @param[in] message printf formatted message.
 * @param[in] ... optional arguments to formatted message.
 */
#ifdef ASSERT_NO_ABORT
#undef ASSERT_NO_ABORT
#endif
#ifdef ENABLE_ASSERTS
# define ASSERT_NO_ABORT(condition, ...) do { if (!(condition)) { char buffer[1024]; snprintf(buffer, sizeof(buffer), __VA_ARGS__);  fprintf(stderr, "[%s:%s]ASSERT:(%s): %s\n", __FILE__, LINE_STRING, #condition, buffer); } } while (0);
#else
# define ASSERT_NO_ABORT(condition, ...)
#endif
/**
 * Abort if condition is false.
 *
 * @param[in] condition the condition to check.
 * @param[in] message printf formatted message.
 * @param[in] ... optional arguments to formatted message.
 */
#ifdef ASSERT_AND_ABORT
#undef ASSERT_AND_ABORT
#endif
#ifdef ENABLE_ASSERTS
# define ASSERT_AND_ABORT(condition, ...) do { ASSERT_NO_ABORT(condition, __VA_ARGS__); ::abort(); } while (0);
#else
# define ASSERT_AND_ABORT(condition, ...)
#endif

/**
 * Abort if condition is false.
 *
 * @param[in] condition.
 * @param[in] message message to output to the console.
 */
#ifdef ASSERT
#undef ASSERT
#endif
#ifdef ENABLE_ASSERTS
#ifdef ASSERTS_ABORT
#  define ASSERT(condition, ...) do { if (!(condition)) ASSERT_AND_ABORT(condition, __VA_ARGS__); } while (0);
#else
#  define ASSERT(condition, ...) do { if (!(condition)) ASSERT_NO_ABORT(condition, __VA_ARGS__); } while (0);
# endif
#else
# define ASSERT(condition, ...)
#endif

#endif // ASSERT_H
