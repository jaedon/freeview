/*=============================================================================
Copyright (c) 2011 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  External interface

FILE DESCRIPTION
Environment configurable, one-shot options.
=============================================================================*/

#include "interface/khronos/common/khrn_options.h"

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef ANDROID
#include <cutils/log.h>
#include <cutils/properties.h>
#endif

KHRN_OPTIONS_T khrn_options;

/* For now, we will use environment variables for the options.
   We might, at some point, want to use ini files perhaps */
static bool read_bool_option(const char *name, bool cur)
{
   char *val = getenv(name);

   if (val == NULL)
      return cur;

   if (val[0] == 't' || val[0] == 'T' || val[0] == '1')
      return true;

   return false;
}

static uint32_t read_uint32_option(const char *name, uint32_t cur)
{
#ifdef ANDROID
   char val[PROPERTY_VALUE_MAX];
   char __default[PROPERTY_VALUE_MAX];

   snprintf(__default, PROPERTY_VALUE_MAX, "%d", cur);

   /* this cant fail, the default is returned if the key doesn't exist */
   property_get(name, val, __default);

   return atoi(val);
#else
   char *val = getenv(name);

   if (val == NULL)
      return cur;

   if (val[0] != '\0')
      return atoi(val);

   return 0;
#endif
}

void khrn_init_options(void)
{
   /* Default values are all 0 */
   memset(&khrn_options, 0, sizeof(KHRN_OPTIONS_T));

#ifndef DISABLE_OPTION_PARSING
   /* Now read the options */
   khrn_options.gl_error_assist        = read_bool_option(  "V3D_GL_ERROR_ASSIST",        khrn_options.gl_error_assist);
   khrn_options.double_buffer          = read_bool_option(  "V3D_DOUBLE_BUFFER",          khrn_options.double_buffer);
   khrn_options.no_bin_render_overlap  = read_bool_option(  "V3D_NO_BIN_RENDER_OVERLAP",  khrn_options.no_bin_render_overlap);
   khrn_options.reg_dump_on_lock       = read_bool_option(  "V3D_REG_DUMP_ON_LOCK",       khrn_options.reg_dump_on_lock);
   khrn_options.clif_dump_on_lock      = read_bool_option(  "V3D_CLIF_DUMP_ON_LOCK",      khrn_options.clif_dump_on_lock);
   khrn_options.force_dither_off       = read_bool_option(  "V3D_FORCE_DITHER_OFF",       khrn_options.force_dither_off);

   khrn_options.bin_block_size         = read_uint32_option("V3D_BIN_BLOCK_SIZE",         khrn_options.bin_block_size);
   khrn_options.max_bin_blocks         = read_uint32_option("V3D_MAX_BIN_BLOCKS",         khrn_options.max_bin_blocks);
#endif

#ifdef ANDROID
   khrn_options.gl_error_assist        = read_uint32_option("debug.gl.hw.gl_error_assist",         khrn_options.gl_error_assist);
   khrn_options.double_buffer          = read_uint32_option("debug.gl.hw.no_bin_render_overlap",   khrn_options.double_buffer);
   khrn_options.reg_dump_on_lock       = read_uint32_option("debug.gl.hw.reg_dump_on_lock",        khrn_options.reg_dump_on_lock);
   khrn_options.force_dither_off       = read_uint32_option("debug.gl.hw.force_dither_off",        khrn_options.force_dither_off);

   khrn_options.bin_block_size         = read_uint32_option("debug.gl.hw.bin_block_size",          khrn_options.bin_block_size);
   khrn_options.max_bin_blocks         = read_uint32_option("debug.gl.hw.max_bin_blocks",          khrn_options.max_bin_blocks);

   LOGD("==========================================================================\n");
   LOGD("debug.gl.hw.gl_error_assist = %d\n", khrn_options.gl_error_assist);
   LOGD("debug.gl.hw.no_bin_render_overlap = %d\n", khrn_options.no_bin_render_overlap);
   LOGD("debug.gl.hw.reg_dump_on_lock = %d\n", khrn_options.reg_dump_on_lock);
   LOGD("debug.gl.hw.force_dither_off = %d\n", khrn_options.force_dither_off);
   LOGD("debug.gl.hw.bin_block_size = %d\n", khrn_options.bin_block_size);
   LOGD("debug.gl.hw.max_bin_blocks = %d\n", khrn_options.max_bin_blocks);
   LOGD("==========================================================================\n");
#endif
}

void khrn_error_assist(GLenum error, const char *func, unsigned int line)
{
   if (khrn_options.gl_error_assist && error != GL_NO_ERROR)
   {
      fprintf(stderr, "V3D ERROR ASSIST : ");
      switch (error)
      {
      case GL_INVALID_ENUM      : fprintf(stderr, "GL_INVALID_ENUM in %s (line %d)\n", func, line); break;
      case GL_INVALID_VALUE     : fprintf(stderr, "GL_INVALID_VALUE in %s (line %d)\n", func, line); break;
      case GL_INVALID_OPERATION : fprintf(stderr, "GL_INVALID_OPERATION in %s (line %d)\n", func, line); break;
      case GL_OUT_OF_MEMORY     : fprintf(stderr, "GL_OUT_OF_MEMORY in %s (line %d)\n", func, line); break;
      default                   : fprintf(stderr, "ERROR CODE %d in %s (line %d)\n", (int)error, func, line); break;
      }
      fflush(stderr);
   }
}