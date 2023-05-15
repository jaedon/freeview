/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NRDP_VERSION_H
#define NRDP_VERSION_H

#undef NRDP_VERSION_MAJOR
#define NRDP_VERSION_MAJOR 2015

#undef NRDP_VERSION_MINOR
#define NRDP_VERSION_MINOR 1

#undef NRDP_VERSION_PATCH
#define NRDP_VERSION_PATCH 1

//#define NRDP_VERSION_SUFFIX "B1"

#undef NRDP_VERSION_NUMBER_STR2
#define NRDP_VERSION_NUMBER_STR2(M) #M
#undef NRDP_VERSION_NUMBER_STR
#define NRDP_VERSION_NUMBER_STR(M) NRDP_VERSION_NUMBER_STR2(M)

#undef NRDP_VERSION_NUMBER
#define NRDP_VERSION_NUMBER NRDP_VERSION_NUMBER_STR(NRDP_VERSION_MAJOR) "." NRDP_VERSION_NUMBER_STR(NRDP_VERSION_MINOR) "." NRDP_VERSION_NUMBER_STR(NRDP_VERSION_PATCH)

#undef NRDP_VERSION_STRING
#ifdef NRDP_VERSION_SUFFIX
# define NRDP_VERSION_STRING NRDP_VERSION_NUMBER "-" NRDP_VERSION_SUFFIX
#else
# define NRDP_VERSION_STRING NRDP_VERSION_NUMBER
#endif

#endif
