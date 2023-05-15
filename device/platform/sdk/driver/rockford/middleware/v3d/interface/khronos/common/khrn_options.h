/*=============================================================================
Copyright (c) 2011 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  External interface

FILE DESCRIPTION
Environment configurable, one-shot options.
=============================================================================*/

#ifndef KHRN_OPTIONS_H
#define KHRN_OPTIONS_H

#include "interface/khronos/common/khrn_int_common.h"
#include "interface/khronos/include/GLES/gl.h"

typedef struct {
   bool     gl_error_assist;           /* Outputs useful info when the error occurs */
   bool     double_buffer;             /* Defaults to triple-buffered */
   bool     no_bin_render_overlap;     /* Prevent bin/render overlap */
   bool     reg_dump_on_lock;          /* Dump h/w registers if the h/w locks-up */
   bool     clif_dump_on_lock;         /* Dump clif file and memory on h/w lock-up */
   bool     force_dither_off;          /* Ensure dithering is always off */
   uint32_t bin_block_size;            /* Set the size of binning memory blocks */
   uint32_t max_bin_blocks;            /* Set the maximum number of binning block in use */

} KHRN_OPTIONS_T;

extern KHRN_OPTIONS_T khrn_options;

extern void khrn_init_options(void);
extern void khrn_error_assist(GLenum error, const char *func, unsigned int line);

#ifdef WIN32
#undef __func__
#define __func__ __FUNCTION__
#endif

#endif
