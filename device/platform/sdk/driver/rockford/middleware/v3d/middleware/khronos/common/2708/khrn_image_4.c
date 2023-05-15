/*=============================================================================
Copyright (c) 2009 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Image

FILE DESCRIPTION
Platform-specific image stuff.
=============================================================================*/

#include "interface/khronos/common/khrn_int_common.h"
#include "middleware/khronos/common/2708/khrn_prod_4.h"
#include "middleware/khronos/common/khrn_image.h"
#include "interface/khronos/common/khrn_int_image.h"
#include "interface/khronos/common/khrn_int_util.h"
#include "middleware/khronos/common/2708/khrn_fmem_4.h"
#include "middleware/khronos/common/2708/khrn_prod_4.h"
#include "middleware/khronos/common/2708/khrn_interlock_priv_4.h"

/*
   is the format ok for rendering to? this is only for color buffer formats, not
   z/stencil formats etc
*/

bool khrn_image_is_ok_for_render_target(KHRN_IMAGE_FORMAT_T format, bool ignore_mem_layout)
{
   vcos_assert(khrn_image_is_color(format));
   switch (format & ~(IMAGE_FORMAT_MEM_LAYOUT_MASK | IMAGE_FORMAT_PRE | IMAGE_FORMAT_LIN)) {
   case ABGR_8888:
   case XBGR_8888:
   case RGBA_8888:
   case RGBX_8888:
   case RGB_565:
      /* need tformat on a0 for vg, but allow rso anyway as it's faster and will
       * only appear when we build with RSO_FRAMEBUFFER defined (in which case
       * we should be aware that some stuff might not work) */
      return ignore_mem_layout || khrn_image_is_tformat(format) || khrn_image_is_rso(format)
         || khrn_image_is_lineartile(format)
         ;
   default:
      return false;
   }
}

/*
   should only be called if khrn_image_is_ok_for_render_target(true) says yes.
   checks the other image properties, eg memory layout, stride, size, alignment.
   again, this is only for color buffers, not z/stencil etc
*/

bool khrn_image_can_use_as_render_target(KHRN_IMAGE_T *image)
{
   vcos_assert(khrn_image_is_ok_for_render_target(image->format, true));
   if (/* check the memory layout */
      !khrn_image_is_ok_for_render_target(image->format, false) ||
      (image->stride & (KHRN_HW_TLB_ALIGN - 1)) ||
      (khrn_image_get_align(image) < KHRN_HW_TLB_ALIGN)) {
      vcos_assert(!(image->flags & IMAGE_FLAG_RENDER_TARGET));
      return false;
   }
   image->flags |= IMAGE_FLAG_RENDER_TARGET;
   return true;
}

/*
   fudge the image parameters to make it ok for the requested usage. the render
   target usage includes z/stencil/mask buffers as well as color buffers
*/

void khrn_image_platform_fudge(
   KHRN_IMAGE_FORMAT_T *format,
   uint32_t *padded_width, uint32_t *padded_height,
   uint32_t *align, KHRN_IMAGE_CREATE_FLAG_T flags)
{
   if (flags & IMAGE_CREATE_FLAG_TEXTURE) {
      if (!(flags & IMAGE_CREATE_FLAG_RSO_TEXTURE)) {
         *format = khrn_image_to_tf_format(*format);
         if (khrn_image_prefer_lt(*format, *padded_width, *padded_height)) {
            *format = khrn_image_to_lt_format(*format);
         }
      }
      else
      {
         /* height multiple of 4 */
         *padded_height = (*padded_height + 3) & ~0x3;
      }
      *align = _max(*align, KHRN_HW_TEX_ALIGN);
   }
   if (flags & IMAGE_CREATE_FLAG_RENDER_TARGET) {
      /* if it looks like a color buffer, check it's one of the supported
       * formats */
      vcos_assert(!khrn_image_is_color(*format) ||
         (*format == A_8_RSO) || /* A_8_RSO used for mask buffers */
         (*format == COL_32_TLBD) || /* COL_32_TLBD used for multisample buffers */
         khrn_image_is_ok_for_render_target(*format, false));
      /* stride needs to be a multiple of KHRN_HW_TLB_ALIGN. this is trivially
       * satisfied for lineartile and tformat as utiles are 16 bytes wide. need
       * to pad raster order widths though... */
      if (khrn_image_is_rso(*format)) {
         *padded_width = round_up(*padded_width, (KHRN_HW_TLB_ALIGN * 8) / khrn_image_get_bpp(*format));
      }
      *align = _max(*align, KHRN_HW_TLB_ALIGN);
   }
   if (flags & IMAGE_CREATE_FLAG_DISPLAY) {
      *align = _max(*align, 4096); /* need 4k alignment for scaler */
   }
}
/* TLB image conversion stuff */
//#define DISABLE_IMAGE_CONVERT_TLB
typedef struct IMAGE_CONVERT_T {
   KHRN_INTERLOCK_T *src_interlock;
   KHRN_INTERLOCK_T *dst_interlock;
   uint32_t src_format;
   uint32_t dst_format;
   uint32_t src_colour_format;
   uint32_t dst_colour_format;
   uint32_t width;
   uint32_t height;
   uint32_t width_in_tiles;
   uint32_t height_in_tiles;
   void *src_storage;
   void *dst_storage;
   KHRN_FMEM_T *fmem;
} IMAGE_CONVERT_T;

#define IMAGE_COPY_FAST_ADDRESS_ALIGN 16
#define IMAGE_COPY_FAST_PIXEL_ALIGN 16
#define TLB_INVALID_FORMAT 3

static uint32_t get_tlb_format(KHRN_IMAGE_FORMAT_T format)
{
   if (khrn_image_is_rso(format)) {
      return 0;
   }
   if (khrn_image_is_tformat(format)) {
      return 1;
   }
   if (khrn_image_is_lineartile(format)) {
      return 2;
   }
   vcos_assert (0);
   return TLB_INVALID_FORMAT;
}

static uint32_t get_tlb_colour_format(KHRN_IMAGE_FORMAT_T format)
{
   switch (format & ~IMAGE_FORMAT_MEM_LAYOUT_MASK) {
   case ABGR_8888:
   case XBGR_8888:
      return 0;
   case RGB_565:
      return 2;
   default:
      return TLB_INVALID_FORMAT;
   }
}

static uint32_t get_tlb_colour_format_conf(uint32_t ls_format)
{
   static uint32_t conv[] = {1, 0, 2, TLB_INVALID_FORMAT};
   vcos_assert (ls_format <= TLB_INVALID_FORMAT);
   return conv[ls_format];
}

static void *hw_get_direct_alias_and_flush(void *p, uint32_t size)
{
   khrn_hw_flush_dcache_range(p,size);
   return (void *)khrn_hw_alias_direct(p);
}

static bool tlb_conv_setup(
   IMAGE_CONVERT_T *image_convert,
   KHRN_IMAGE_WRAP_T *dst,
   uint32_t dst_x, uint32_t dst_y,
   uint32_t width, uint32_t height,
   const KHRN_IMAGE_WRAP_T *src,
   uint32_t src_x, uint32_t src_y)
{
   uint32_t src_bpp, dst_bpp, stride;

   image_convert->src_format = get_tlb_format(src->format);
   image_convert->dst_format = get_tlb_format(dst->format);
   if ((image_convert->src_format == TLB_INVALID_FORMAT) || (image_convert->dst_format == TLB_INVALID_FORMAT)) {
      return false;
   }

   if ((!khrn_image_is_rso(src->format) && ((src_x != 0) || (src_y != 0))) ||
      (!khrn_image_is_rso(dst->format) && ((dst_x != 0) || (dst_y != 0)))) {
      /* todo: make this less restrictive */
      return false;
   }
   
   src_bpp = khrn_image_get_bpp(src->format) / 8;
   dst_bpp = khrn_image_get_bpp(src->format) / 8;

   if ((src->format & ~IMAGE_FORMAT_MEM_LAYOUT_MASK) == (dst->format & ~IMAGE_FORMAT_MEM_LAYOUT_MASK)) {
      switch (src_bpp) {
      case 4:
         /* any valid 32-bit format OK */
         image_convert->src_colour_format = image_convert->dst_colour_format = 
            get_tlb_colour_format(ABGR_8888); 
         break;
      case 2:
         /* any valid 16-bit format OK */
         image_convert->src_colour_format = image_convert->dst_colour_format = 
            get_tlb_colour_format(RGB_565);
         break;
      default:
         return false;
      }
   } else {
      image_convert->src_colour_format = get_tlb_colour_format(src->format);
      image_convert->dst_colour_format = get_tlb_colour_format(dst->format);
      if ((image_convert->src_colour_format == TLB_INVALID_FORMAT) || (image_convert->dst_colour_format == TLB_INVALID_FORMAT)) {
            return false;
      }
   }

   if ((dst->stride & (IMAGE_COPY_FAST_PIXEL_ALIGN-1)) ||
      (src->stride & (IMAGE_COPY_FAST_PIXEL_ALIGN-1)) ||
      (src->stride != dst->stride)) {
      return false;
   }
   stride = src->stride / src_bpp;
   if (stride != dst->stride / dst_bpp) {
      return false;
   }
   image_convert->width_in_tiles = (width + (KHRN_HW_TILE_WIDTH - 1)) >> KHRN_HW_LOG2_TILE_WIDTH;
   image_convert->height_in_tiles = (height + (KHRN_HW_TILE_HEIGHT - 1)) >> KHRN_HW_LOG2_TILE_HEIGHT;
   if ((width != stride) && (width != image_convert->width_in_tiles * KHRN_HW_TILE_WIDTH)) {
      return false;
   }

   image_convert->src_storage = (uint8_t *)src->storage + 
      (src_y * src->stride) + (src_bpp * src_x);
   image_convert->dst_storage = (uint8_t *)dst->storage + 
      (dst_y * dst->stride) + (dst_bpp * dst_x);
   if (((uint32_t)(image_convert->src_storage) & IMAGE_COPY_FAST_ADDRESS_ALIGN-1) ||
      ((uint32_t)(image_convert->dst_storage) & IMAGE_COPY_FAST_ADDRESS_ALIGN-1)) {
      return false;
   }

   image_convert->width = stride;
   image_convert->height = height;

   /* todo: needs work when running with an MMU in RPC direct mode */
   image_convert->src_storage = hw_get_direct_alias_and_flush(image_convert->src_storage, src->stride * height);
   image_convert->dst_storage = hw_get_direct_alias_and_flush(image_convert->dst_storage, dst->stride * height);

   return true;
}

bool khrn_hw_convert_image_cli(IMAGE_CONVERT_T *image_convert)
{
   uint8_t *p;
   uint32_t i, j;
   void *data;

   image_convert->fmem = khrn_fmem_init(KHRN_INTERLOCK_USER_TEMP);
   if (!image_convert->fmem) { goto error_ret; }
   if (!khrn_fmem_start_render(image_convert->fmem)) { goto error_free; }

   khrn_interlock_read(image_convert->src_interlock, KHRN_INTERLOCK_USER_TEMP);
   khrn_interlock_write(image_convert->dst_interlock, KHRN_INTERLOCK_USER_TEMP);

   /* set rendering mode config */
   p = khrn_fmem_cle(image_convert->fmem, 11);
   ADD_BYTE(p, KHRN_HW_INSTR_STATE_TILE_RENDERING_MODE);
   ADD_WORD(p, khrn_hw_addr(image_convert->dst_storage));

   ADD_SHORT(p, (uint16_t)image_convert->width);
   ADD_SHORT(p, (uint16_t)image_convert->height);
   ADD_BYTE(p,
      (0 << 0) | /* disable ms mode */
      (0 << 1) | /* disable 64-bit color mode */
      (get_tlb_colour_format_conf(image_convert->dst_colour_format) << 2) |
      (0 << 4) | /* No decimation */
      (image_convert->dst_format << 6)); /* T-format format */
   ADD_BYTE(p,
      (0 << 0) | /* no vg mask */
      (0 << 1) | /* no coverage mode */
      (0 << 2) | /* don't care z-update dir */
      (0 << 3) | /* disable early Z */
      (0 << 4)); /* no double buffer */

   for (j = 0; j != image_convert->height_in_tiles; ++j) {
      for (i = 0; i != image_convert->width_in_tiles; ++i) {
         bool eof = (i == (image_convert->width_in_tiles - 1)) && (j == (image_convert->height_in_tiles - 1));

         /* todo: fewer, larger allocations? */
         p = khrn_fmem_cle(image_convert->fmem, 11);
         ADD_BYTE(p, KHRN_HW_INSTR_LOAD_GENERAL);
         ADD_BYTE(p, (uint8_t)(
            (1 << 0) | /* load color */
            ((image_convert->src_format) << 4)
         ));
         ADD_BYTE(p, image_convert->src_colour_format);
         ADD_WORD(p,khrn_hw_addr(image_convert->src_storage));

         ADD_BYTE(p, KHRN_HW_INSTR_STATE_TILE_COORDS);
         ADD_BYTE(p, (uint8_t)i);
         ADD_BYTE(p, (uint8_t)j);
         
         ADD_BYTE(p, eof ? KHRN_HW_INSTR_STORE_SUBSAMPLE_EOF : KHRN_HW_INSTR_STORE_SUBSAMPLE);
         vcos_assert(khrn_fmem_is_here(image_convert->fmem, p));
      }
   }
   /*
      pass to hw...
   */

   khrn_hw_advance_enter_pos();

   /* transfer buffers */
   khrn_interlock_transfer(image_convert->dst_interlock,
      KHRN_INTERLOCK_USER_TEMP, KHRN_INTERLOCK_FIFO_HW_RENDER);
   khrn_interlock_transfer(image_convert->src_interlock,
      KHRN_INTERLOCK_USER_TEMP, KHRN_INTERLOCK_FIFO_HW_RENDER);

   data = khrn_fmem_queue(image_convert->fmem,
      KHRN_HW_CC_FLAG_NONE,
      (KHRN_HW_CC_FLAG_T)(KHRN_HW_CC_FLAG_L2 | KHRN_HW_CC_FLAG_IC | KHRN_HW_CC_FLAG_UC | KHRN_HW_CC_FLAG_TU),
      1, /* frame count */
      0, /* no special stuff */
      0, /* bin mem size */
      0, /* actual VPM ? */
      KHRN_HW_VPM_BLOCKS_N, /* main hw doesn't need any vpm  */
      KHRN_HW_TYPE_VG,
      NULL, 0);

   khrn_hw_ready(true, data);
   return true;
error_free:
   khrn_fmem_discard(image_convert->fmem);
error_ret:
   return false;
}

bool khrn_image_wrap_copy_region_tlb(
   KHRN_IMAGE_WRAP_T *dst, uint32_t dst_x, uint32_t dst_y,
   uint32_t width, uint32_t height,
   const KHRN_IMAGE_WRAP_T *src, uint32_t src_x, uint32_t src_y,
   KHRN_IMAGE_CONV_T conv)
{
   bool result = false;
   IMAGE_CONVERT_T image_convert;
   UNUSED(conv);

   vcos_assert (dst->interlock != NULL);
   vcos_assert (src->interlock != NULL);
   vcos_assert((dst_x + width) <= dst->width);
   vcos_assert((dst_y + height) <= dst->height);
   vcos_assert((src_x + width) <= src->width);
   vcos_assert((src_y + height) <= src->height);
   
   if (tlb_conv_setup(&image_convert, dst, dst_x, dst_y, width, height, src, src_x, src_y)) {
      image_convert.dst_interlock = dst->interlock;
      image_convert.src_interlock = src->interlock;
#ifndef DISABLE_IMAGE_CONVERT_TLB
      result = khrn_hw_convert_image_cli(&image_convert);
#endif
   }

   return result;
}