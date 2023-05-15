/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BREAKPOINTS_H_
#define BREAKPOINTS_H_

#if __i386__
# define HARD_BREAKPOINT asm volatile ("int3;");
# define BREAKPOINT \
    asm("0:"                              \
        ".pushsection embed-breakpoints;" \
        ".quad 0b;"                       \
        ".popsection;")

#elif __arm__
# define HARD_BREAKPOINT asm("bkpt;");
# define BREAKPOINT

#else
# define HARD_BREAKPOINT
# define BREAKPOINT

#endif

#endif /* BREAKPOINTS_H_ */

