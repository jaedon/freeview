/*=============================================================================
Copyright (c) 2012 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  External interface
File     :  $RCSfile$
Revision :  $Revision$

FILE DESCRIPTION
Workarounds interface.
=============================================================================*/

#include "interface/khronos/common/khrn_int_common.h"
#include "interface/khronos/include/EGL/egl.h"
#include "vcfw/drivers/chip/abstract_v3d.h"

#include <stdio.h>

/* The workarounds table */
KHRN_WORKAROUNDS_T khrn_workarounds;

void khrn_init_workarounds(void)
{
   /* Read the IDENT registers and get the information we need for the workarounds */
   uint32_t ident0         = v3d_read(IDENT0);
   uint32_t ident1         = v3d_read(IDENT1);
   uint32_t ident2         = v3d_read(IDENT2);
   uint32_t numSlices      = (ident1 >> 4) & 0xF;
   uint32_t tmusPerSlice   = (ident1 >> 12) & 0xF;
   uint32_t techRev        = (ident0 >> 24);
   uint32_t revision       = (ident1 & 0xF);

   if (ident0 == 0 || ident1 == 0 || ident2 == 0)
   {
      /* Couldn't read the registers */
      fprintf(stderr, "*** ERROR : 3d driver couldn't read IDENT registers. ***\n");
   }

   /* techRev  : 2=A, 3=B */
   /* revision : 0=0, 1=1, 2=2 */

   /* All workarounds on by default */
   memset(&khrn_workarounds, (int)true, sizeof(khrn_workarounds));

   khrn_workarounds.TMUS_PER_SLICE = tmusPerSlice;
   khrn_workarounds.FB_BOTTOM_UP   = true;
   khrn_workarounds.FB_TOP_DOWN    = false;
   khrn_workarounds.HAS_32BIT_SUPPORT = false;

   if (techRev >= 3 && revision >= 1)
   {
      /* Features at B1 or above */
      khrn_workarounds.FB_BOTTOM_UP = false;
      khrn_workarounds.FB_TOP_DOWN  = true;

      /* Fixed at B1 or above */
      khrn_workarounds.HW2924 = false;    /* Fwd/rev stencil speckles */
   }
   
   if (techRev >= 3 && revision >= 2)
   {
      /* Features at B2 or above */
      khrn_workarounds.HAS_32BIT_SUPPORT = true;

      /* Fixed at B2 or above */
      khrn_workarounds.HW2116 = false;    /* let ptb state counters wrap around safely */
      khrn_workarounds.HW2806 = false;    /* Flags get written back to wrong quad when z test result collides with new thread */
      khrn_workarounds.HW2885 = false;    /* lockup due to erroneous activity in varyings interpolation module when outputting to the coverage pipe */
      khrn_workarounds.HW2898 = false;    /* NPOT raster textures */
      khrn_workarounds.HW2903 = false;    /* Zero-size points break PTB */
      khrn_workarounds.HW2905 = false;    /* Multisample full depth load breaks early-z */
      khrn_workarounds.HW2924 = false;    /* Fwd/rev stencil speckles */
      khrn_workarounds.SBWAIT = false;    /* core has to issue a SBWAIT instruction to access the scoreboard */
      khrn_workarounds.GFXH724 = false;   /* Make top bits of INTCTL read masked interrupt status */
   }
}
