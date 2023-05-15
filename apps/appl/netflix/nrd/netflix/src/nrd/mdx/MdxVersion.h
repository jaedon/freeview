/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MDXLIB_VERSION_H
#define MDXLIB_VERSION_H

#undef MDXLIB_VERSION_MAJOR
#define MDXLIB_VERSION_MAJOR 2015

#undef MDXLIB_VERSION_MINOR
#define MDXLIB_VERSION_MINOR 1

#undef MDXLIB_VERSION_PATCH
#define MDXLIB_VERSION_PATCH 0

#undef MDXLIB_VERSION_NUMBER_STR2
#define MDXLIB_VERSION_NUMBER_STR2(M) #M
#undef MDXLIB_VERSION_NUMBER_STR
#define MDXLIB_VERSION_NUMBER_STR(M) MDXLIB_VERSION_NUMBER_STR2(M)

#undef MDXLIB_VERSION_NUMBER
#define MDXLIB_VERSION_NUMBER MDXLIB_VERSION_NUMBER_STR(MDXLIB_VERSION_MAJOR) "." MDXLIB_VERSION_NUMBER_STR(MDXLIB_VERSION_MINOR) "." MDXLIB_VERSION_NUMBER_STR(MDXLIB_VERSION_PATCH)

#undef MDXLIB_VERSION_STRING
#ifdef MDXLIB_VERSION_SUFFIX
# define MDXLIB_VERSION_STRING MDXLIB_VERSION_NUMBER "-" MDXLIB_VERSION_SUFFIX
#else
# define MDXLIB_VERSION_STRING MDXLIB_VERSION_NUMBER
#endif

#endif
