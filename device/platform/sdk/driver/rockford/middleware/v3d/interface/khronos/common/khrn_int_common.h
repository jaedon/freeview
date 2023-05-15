/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Common include

FILE DESCRIPTION
Common include.
=============================================================================*/

#ifndef KHRN_INT_COMMON_H
#define KHRN_INT_COMMON_H
#ifdef __cplusplus
extern "C" {
#endif\

//#define KHRN_NOT_REALLY_DUALCORE   // Use dual core codebase but switch master thread to vpu1
//#define KHRN_SIMPLE_MULTISAMPLE
//#define USE_CTRL_FOR_DATA

//#define GLXX_FORCE_MULTISAMPLE

/* As BCG runs from cached memory, all allocations have to be the max of the CPU cache (calculated in the platform layer) and
   the size of the cache line on the L3 */
#define BCG_GCACHE_LINE_SIZE        256

#include <limits.h>
#define KHDISPATCH_WORKSPACE_SIZE INT_MAX

#ifndef NULL
# ifdef __cplusplus
#  define NULL 0
# else
#  define NULL ((void *)0)
# endif
#endif

#include "interface/vcos/vcos_assert.h"
#include <string.h> /* size_t */

#ifdef _MSC_VER
#define INLINE __inline
typedef unsigned long long uint64_t;
#else
#ifdef __GNUC__
/* Just using inline doesn't work for gcc (at least on MIPS), so use the gcc attribute.
   This gives a pretty decent performance boost */
#define INLINE __attribute__((always_inline)) __attribute__((no_instrument_function))
#else
#define INLINE inline
#endif
#endif

#ifndef __mips__
#ifndef __cplusplus
   typedef uint8_t bool;
   #define false 0
   #define true 1
#endif
#else
#include <stdbool.h>
#endif

#ifdef NDEBUG
   #define verify(X) X
#else
   #define verify(X) vcos_assert(X)
#endif
#define UNREACHABLE() vcos_assert(0)

#ifdef _MSC_VER
   #define UNUSED(X) X
#else
   #define UNUSED(X)
#endif

#ifdef NDEBUG
   #define UNUSED_NDEBUG(X) UNUSED(X)
#else
   #define UNUSED_NDEBUG(X)
#endif

#define KHRN_NO_SEMAPHORE 0xffffffff

typedef struct {
   /* Features */
   uint32_t TMUS_PER_SLICE;   /* Number of TMUs per slice */
   bool     FB_BOTTOM_UP;     /* Bottom up framebuffer */
   bool     FB_TOP_DOWN;      /* Top down framebuffer */
   bool     HAS_32BIT_SUPPORT;/* Supports 32-bit addressing */

   /* Workarounds */
   bool GFXH16;   /* System can get stuck in loop where icache loads are evicted before they can be used */
   bool GFXH30;   /* VCM can get stuck on big string of empty tiles */
   bool GFXH776;  /* CLE pre-fetches data beyond the end of index lists */
   bool HW2116;   /* let ptb state counters wrap around safely */
   bool HW2806;   /* Flags get written back to wrong quad when z test result collides with new thread */
   bool HW2885;   /* lockup due to erroneous activity in varyings interpolation module when outputting to the coverage pipe */
   bool HW2898;   /* NPOT raster textures */
   bool HW2903;   /* Zero-size points break PTB */
   bool HW2905;   /* Multisample full depth load breaks early-z */
   bool HW2924;   /* Fwd/rev stencil speckles */
   bool HW2989;   /* only set stencil mode prior to last thrsw */
   bool SBWAIT;   /* core has to issue a SBWAIT instruction to access the scoreboard */
   bool GFXH724;  /* Make top bits of INTCTL read masked interrupt status */
} KHRN_WORKAROUNDS_T;

extern KHRN_WORKAROUNDS_T khrn_workarounds;
extern void khrn_init_workarounds(void);

#ifdef __cplusplus
 }
#endif

#endif

