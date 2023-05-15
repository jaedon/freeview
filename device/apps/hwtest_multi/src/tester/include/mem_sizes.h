/*
 * Very simple but very effective user-space memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <memtest@discworld.dyndns.org>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004 Charles Cazabon <memtest@discworld.dyndns.org>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 * This file contains some macro definitions for handling 32/64 bit platforms.
 *
 */

#include <limits.h>

#define rand32() ((unsigned int) rand() | ( (unsigned int) rand() << 16))

/*
#if (ULONG_MAX == 4294967295)
*/

    #define rand_ul() rand32()
    #define UL_ONEBITS 0xffffffff
    #define UL_LEN 32
    #define CHECKERBOARD1 0x55555555
    #define CHECKERBOARD2 0xaaaaaaaa
    #define UL_BYTE(x) ((x | x << 8 | x << 16 | x << 24))
/*

#elif (ULONG_MAX == 18446744073709551615)
    #define rand64() (((unsigned long) rand32()) << 32 | ((unsigned long) rand32()))
    #define rand_ul() rand64()
    #define UL_ONEBITS 0xffffffffffffffffUL
    #define UL_LEN 64
    #define CHECKERBOARD1 0x5555555555555555
    #define CHECKERBOARD2 0xaaaaaaaaaaaaaaaa
    #define UL_BYTE(x) (((unsigned long)x | (unsigned long)x<<8 | (unsigned long)x<<16 | (unsigned long)x<<24 | (unsigned long)x<<32 | (unsigned long)x<<40 | (unsigned long)x<<48 | (unsigned long)x<<56))
#else
    #error long on this platform is not 32 or 64 bits
#endif

*/
