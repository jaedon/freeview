#ifndef StdInt_h
#define StdInt_h

/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/* We don't want to use ullong/llong because they're typedeffed to
   (unsigned) long long on 32 bit and (unsigned) long on 64 bit systems. This
   means we can't portably printf them unless we use PRId64 which is positively
   awful. Since (unsigned) long always is the same sice as (unsigned) long long
   on 64-bit systems we just use n(u)int64 instead and we can better printf
   portably. */


#include <stdint.h>

typedef unsigned long long ullong;
typedef long long llong;

#endif
