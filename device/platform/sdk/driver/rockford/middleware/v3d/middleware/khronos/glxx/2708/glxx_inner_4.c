/*=============================================================================
Copyright (c) 2009 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Header file

FILE DESCRIPTION
Functions for driving the hardware for both GLES1.1 and GLES2.0.
=============================================================================*/

#include "interface/khronos/common/khrn_int_common.h"
#include "interface/khronos/common/khrn_int_color.h"
#include "interface/khronos/common/khrn_options.h"

#include "middleware/khronos/glxx/glxx_hw.h"
#include "middleware/khronos/glxx/2708/glxx_inner_4.h"
#include "middleware/khronos/glxx/2708/glxx_tu_4.h"
#include "interface/khronos/include/GLES2/gl2.h"
#include "interface/khronos/include/EGL/eglext.h"
#include "interface/khronos/common/khrn_api_interposer.h"

//XXXXXXXXXXXXXXXX
#include "middleware/khronos/egl/egl_platform.h"
#include "middleware/khronos/egl/egl_server.h"
#include "middleware/khronos/common/khrn_hw.h"
#include "middleware/khronos/common/khrn_tformat.h"
#include "middleware/khronos/common/khrn_interlock.h"
#include "middleware/khronos/common/2708/khrn_pool_4.h"
#include "middleware/khronos/common/2708/khrn_interlock_priv_4.h"
#include "middleware/khronos/common/2708/khrn_render_state_4.h"
#include "middleware/khronos/common/2708/khrn_fmem_4.h"

#if defined(ANDROID)
#include <cutils/log.h>
#endif

/*************************************************************
 Defines
 *************************************************************/

/*************************************************************
 Static data
 *************************************************************/

static GLXX_HW_RENDER_STATE_T *render_state = NULL;

#define stencil_clear_mode(value) \
   ((value)<<8 | /* Stencil value */\
   7<<16 |  /* func : not equal */\
   2<<22 |  /* zpop : pass op = replace */\
   3<<28 |  /* wmc : write mask = 0xFF */\
   3<<30    /* wsel : set both front & back states */)

#define stencil_keep_mode 0

#define stencil_mask_mode(mask) \
   (((mask) & 0xFF) |        /* front mask */\
   (((mask) & 0xFF) << 8)    /* back mask */)

/* Clear depth & stencil (depth & stencil optional depending on uniforms and z-func)
 * Uniform 0 = stencil mode (depends on whether stencil is being cleared)
 * Uniform 1 = stencil write mask
 * z-func set depending on whether depth is being cleared */
static uint32_t clear_shader_ds[] =
{
   /* generated 2010-05-11 11:08:25 */
   0x15827d80, 0x10020ae7, /* mov  stencil, unif */
   0x15827d80, 0x10020ae7, /* mov  stencil, unif */
   0x159cffc0, 0x40020b27, /* mov  tlbz, z       ; sbwait */
   0x009e7000, 0x500009e7, /* nop                ; sbdone */
   0x009e7000, 0x300009e7, /* nop                ; thrend */
   0x009e7000, 0x100009e7, /* nop */
   0x009e7000, 0x100009e7, /* nop */
};

/* Clear depth, stencil & colour (depth & stencil optional depending on uniforms and z-func)
 * Uniform 0 = stencil mode (depends on whether stencil is being cleared)
 * Uniform 1 = stencil write mask
 * z-func set depending on whether depth is being cleared */
static uint32_t clear_shader_dsc[] =
{
   /* generated 2010-05-11 11:08:25 */
   0x15827d80, 0x10020ae7, /* mov  stencil, unif */
   0x15827d80, 0x10020ae7, /* mov  stencil, unif */
   0x159cffc0, 0x40020b27, /* mov  tlbz, z       ; sbwait */
   0x15827d80, 0x10020ba7, /* mov  tlbc, unif */
   0x009e7000, 0x500009e7, /* nop                ; sbdone */
   0x009e7000, 0x300009e7, /* nop                ; thrend */
   0x009e7000, 0x100009e7, /* nop */
   0x009e7000, 0x100009e7, /* nop */
};

/* Clear depth, stencil & colour (masked) (depth & stencil optional depending on uniforms and z-func)
 * Uniform 0 = colour mask
 * Uniform 1 = stencil mode (depends on whether stencil is being cleared)
 * Uniform 2 = stencil write mask
 * z-func set depending on whether depth is being cleared */
static uint32_t clear_shader_pdsc[] =
{
   0x15827d80, 0x10020867, /* mov  r1, unif*/
   0x15827d80, 0x10020ae7, /* mov  stencil, unif*/
   0x15827d80, 0x10020ae7, /* mov  stencil, unif*/
   0x179e7240, 0x40020827, /* not  r0, r1             ; sbwait*/
   0x14827380, 0x80020867, /* and  r1, r1, unif  ; loadc*/
   0x149e7100, 0x10020827, /* and  r0, r0, r4*/
   0x159cffc0, 0x10020b27, /* mov  tlbz, z*/
   0x159e7040, 0x10020ba7, /* or   tlbc, r0, r1*/
   0x009e7000, 0x500009e7, /* nop                ; sbdone*/
   0x009e7000, 0x300009e7, /* nop                ; thrend*/
   0x009e7000, 0x100009e7, /* nop*/
   0x009e7000, 0x100009e7, /* nop*/
};

/* Clear depth, stencil & colour (masked & multisampled) (depth & stencil optional depending on uniforms and z-func)
 * Uniform 0 = colour mask
 * Uniform 1 = stencil mode (depends on whether stencil is being cleared)
 * Uniform 2 = stencil write mask
 * z-func set depending on whether depth is being cleared */
static uint32_t clear_shader_mpdsc[] =
{
   0x15827d80, 0x10020867, /* mov  r1, unif*/
   0x15827d80, 0x10020ae7, /* mov  stencil, unif*/
   0x15827d80, 0x10020ae7, /* mov  stencil, unif*/
   0x179e7240, 0x40020827, /* not  r0, r1        ; sbwait*/
   0x14827380, 0x80020867, /* and  r1, r1, unif  ; loadc*/
   0x149e7100, 0x100208a7, /* and  r2, r0, r4*/
   0x159cffc0, 0x10020b27, /* mov  tlbz, z*/
   0x159e7440, 0x80020b67, /* or   tlbm, r2, r1  ; loadc*/
   0x149e7100, 0x100208a7, /* and  r2, r0, r4*/
   0x159e7440, 0x80020b67, /* or   tlbm, r2, r1  ; loadc*/
   0x149e7100, 0x100208a7, /* and  r2, r0, r4*/
   0x159e7440, 0x80020b67, /* or   tlbm, r2, r1  ; loadc*/
   0x149e7100, 0x100208a7, /* and  r2, r0, r4*/
   0x159e7440, 0x10020b67, /* or   tlbm, r2, r1*/
   0x009e7000, 0x500009e7, /* nop                ; sbdone*/
   0x009e7000, 0x300009e7, /* nop                ; thrend*/
   0x009e7000, 0x100009e7, /* nop*/
   0x009e7000, 0x100009e7, /* nop*/
};

/* Clear colour
 * Uniform 0 = color */
static uint32_t clear_shader_c[] =
{
   /* generated 2010-05-11 11:08:25 */
   0x009e7000, 0x100009e7, /* nop */
   0x009e7000, 0x100009e7, /* nop */
   0x15827d80, 0x40020ba7, /* mov  tlbc, unif    ; sbwait */
   0x009e7000, 0x500009e7, /* nop                ; sbdone */
   0x009e7000, 0x300009e7, /* nop                ; thrend */
   0x009e7000, 0x100009e7, /* nop */
   0x009e7000, 0x100009e7, /* nop */
};

/* Clear colour (masked)
 * Uniform 0 = mask
 * Uniform 1 = color */
static uint32_t clear_shader_pc[] =
{
   0x009e7000, 0x100009e7, /* nop*/
   0x15827d80, 0x10020867, /* mov  r1, unif*/
   0x179e7240, 0x40020827, /* not  r0, r1        ; sbwait*/
   0x14827380, 0x80020867, /* and  r1, r1, unif  ; loadc*/
   0x149e7100, 0x10020827, /* and  r0, r0, r4*/
   0x159e7040, 0x10020ba7, /* or   tlbc, r0, r1*/
   0x009e7000, 0x500009e7, /* nop                ; sbdone*/
   0x009e7000, 0x300009e7, /* nop                ; thrend*/
   0x009e7000, 0x100009e7, /* nop*/
   0x009e7000, 0x100009e7, /* nop*/
};

/* Clear colour (masked & multisampled)
 * Uniform 0 = mask
 * Uniform 1 = color */
static uint32_t clear_shader_mpc[] =
{
   0x009e7000, 0x100009e7, /* nop*/
   0x15827d80, 0x10020867, /* mov  r1, unif*/
   0x179e7240, 0x40020827, /* not  r0, r1        ; sbwait*/
   0x14827380, 0x80020867, /* and  r1, r1, unif  ; loadc*/
   0x149e7100, 0x100208a7, /* and  r2, r0, r4*/
   0x159e7440, 0x80020b67, /* or   tlbm, r2, r1  ; loadc*/
   0x149e7100, 0x100208a7, /* and  r2, r0, r4*/
   0x159e7440, 0x80020b67, /* or   tlbm, r2, r1  ; loadc*/
   0x149e7100, 0x100208a7, /* and  r2, r0, r4*/
   0x159e7440, 0x80020b67, /* or   tlbm, r2, r1  ; loadc*/
   0x149e7100, 0x100208a7, /* and  r2, r0, r4*/
   0x159e7440, 0x10020b67, /* or   tlbm, r2, r1*/
   0x009e7000, 0x500009e7, /* nop                ; sbdone*/
   0x009e7000, 0x300009e7, /* nop                ; thrend*/
   0x009e7000, 0x100009e7, /* nop*/
   0x009e7000, 0x100009e7, /* nop*/
};

#define CLR_COLOR   1
#define CLR_DEPTH   2
#define CLR_STENCIL 4
#define CLR_MASKED  8
#define CLR_MULTISAMPLE  16

static uint32_t *clear_shaders[] =
{
   0,                /* Non-masked, non-multisample */
   clear_shader_c,
   clear_shader_ds,
   clear_shader_dsc,
   clear_shader_ds,
   clear_shader_dsc,
   clear_shader_ds,
   clear_shader_dsc,
   0,                /* Masked, non-multisample */
   clear_shader_pc,
   clear_shader_ds,
   clear_shader_pdsc,
   clear_shader_ds,
   clear_shader_pdsc,
   clear_shader_ds,
   clear_shader_pdsc,
   0,                /* Non-masked, multisample */
   clear_shader_c,
   clear_shader_ds,
   clear_shader_dsc,
   clear_shader_ds,
   clear_shader_dsc,
   clear_shader_ds,
   clear_shader_dsc,
   0,                /* Masked, multisample */
   clear_shader_mpc,
   clear_shader_ds,
   clear_shader_mpdsc,
   clear_shader_ds,
   clear_shader_mpdsc,
   clear_shader_ds,
   clear_shader_mpdsc,
};

static uint32_t clear_shader_sizes[] =
{
   0,
   sizeof(clear_shader_c),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_dsc),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_dsc),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_dsc),
   0,
   sizeof(clear_shader_pc),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_pdsc),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_pdsc),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_pdsc),
   0,
   sizeof(clear_shader_c),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_dsc),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_dsc),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_dsc),
   0,
   sizeof(clear_shader_mpc),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_mpdsc),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_mpdsc),
   sizeof(clear_shader_ds),
   sizeof(clear_shader_mpdsc),
};

static uint32_t null_user_shader[] =
{
   0x009e7000,0x300249e7, // nop             ; thrend
   0x009e7000,0x100249e7, // nop
   0x009e7000,0x100249e7, // nop
};

#ifdef XXX_OFFLINE
static GLXX_FIXABLE_ADDR_T next_user_shader;
static GLXX_FIXABLE_ADDR_T next_user_unif;
static GLXX_FIXABLE_ADDR_T next_vcd_addr;
#endif

/*************************************************************
 Static function forwards
 *************************************************************/
static bool draw_rect(GLXX_SERVER_STATE_T *state, GLXX_HW_RENDER_STATE_T *rs, GLXX_HW_FRAMEBUFFER_T *fb, bool color, bool depth, bool stencil, int x, int y, int xmax, int ymax);
static bool populate_master_cl(GLXX_HW_FRAMEBUFFER_T *fb);
static bool create_bin_cl(void);
static bool create_master_cl(void);

/*************************************************************
 Global Functions
 *************************************************************/


#ifdef XXX_OFFLINE
GLXX_FIXABLE_ADDR_T glxx_hw_get_next_user_shader()
{
   if(next_user_shader.handle == MEM_INVALID_HANDLE)
   {
      void * locked_addr;
      /* Add terminating user shader */
      GLXX_FIXABLE_ADDR_T addr;
      if(glxx_alloc_junk_mem(&addr,sizeof(null_user_shader), 8))
      {
         locked_addr = (char *)mem_lock(addr.handle) + addr.offset;

         next_user_shader.handle = addr.handle;
         next_user_shader.offset = addr.offset;
         next_user_shader.fixable_type = FIXABLE_TYPE_JUNK;

         khrn_memcpy(locked_addr, null_user_shader, sizeof(null_user_shader));
         mem_unlock(addr.handle);
      }
      else
      {
         next_user_shader.handle = MEM_INVALID_HANDLE;
         next_user_shader.offset = 0;
      }
   }
   return next_user_shader;
}

GLXX_FIXABLE_ADDR_T glxx_hw_get_next_user_unif()
{
   return next_user_unif;
}
#endif

/*!
 * \brief Clears state and frame information to start a new frame.
 *
 * This function is used to initialise the simulator hardware for a new
 * frame. It resets the state of various registers, all control lists, the texture
 * handle list and the frame data pointer. It also emits a clear instruction into
 * the new, blank master control list so that the frame buffer is cleared.
 *
 * \param color   is the clear colour to be used.
 * \param depth   is the clear Z depth.
 * \param stencil is the clear stencil to be used.
 * \param state   is the OpenGL server state.
 */
bool glxx_hw_clear(bool color, bool depth, bool stencil, GLXX_SERVER_STATE_T *state)
{
   GLXX_HW_FRAMEBUFFER_T fb;
   uint32_t        cmask = 0;
   GLXX_HW_RENDER_STATE_T *rs;
   bool result;
   bool useDrawRect = false;
   bool clearingAllBuffers = true;
   bool aux_storage = false;
   int x, y, xmax, ymax;

   //moved install frame buffer until after the check

   depth &= state->depth_mask;

   /* Need to check for nothing to clear again here, since the depthMask may have changed the
    * set of buffers to be cleared */
   if (!color && !depth && !stencil)
      return true;

   rs = glxx_install_framebuffer(state, &fb, false);
   if (!rs)
      return true;    /* TODO: or false? */

#ifdef DRAW_TEX_LOGGING
   printf("state_name: %d rs: %d clear: color %d depth %d stencil %d\n",
      state->name,rs->name, color, depth, stencil);
   printf("--------------------\n");
#endif

   if (fb.mh_aux_image != MEM_INVALID_HANDLE)
   {
      KHRN_IMAGE_T *aux_image = (KHRN_IMAGE_T *)mem_lock(fb.mh_aux_image);
      aux_storage = (aux_image->mh_storage != MEM_INVALID_HANDLE);
      mem_unlock(fb.mh_aux_image);
   }

   x = 0;
   y = 0;
   xmax = fb.width;
   ymax = fb.height;

   if (state->caps.scissor_test)
   {
      x = _max(x, state->scissor.x);
      y = _max(y, state->scissor.y);
      xmax = _min(xmax, state->scissor.x + state->scissor.width);
      ymax = _min(ymax, state->scissor.y + state->scissor.height);
      if (x >= xmax || y >= ymax) return true;  /* Successfully cleared nothing */
   }

   /* Depth/Stencil may contain valid info if there is storage or if some was drawn there */
   if (!color || state->shader.common.blend.color_mask != 0xFFFFFFFF)
      clearingAllBuffers = false;
   if (fb.have_depth && !depth && (aux_storage || rs->drawn) )
      clearingAllBuffers = false;
   if ((fb.have_stencil && rs->stencil_used) && (!stencil || (state->stencil_mask.front & 0xFF) != 0xFF) && (aux_storage || rs->drawn) )
      clearingAllBuffers = false;
   if (x != 0 || y != 0 || xmax != fb.width || ymax != fb.height)
      clearingAllBuffers = false;

   rs->stencil_used |= stencil;     /* Make sure a subsequent no-stencil-clear won't overwrite the clear value */

   if (rs->drawn) {
      /* If we're clearing everything clean out the fmem and control list then do a h/w clear */
      if (clearingAllBuffers) {
         rs->hw_frame_count = 0;
         khrn_fmem_discard(rs->fmem);           /* Discard our data and reallocate. */
         rs->fmem = khrn_fmem_init(khrn_interlock_user(rs->name));   /* TODO: Possibly a better way      */
         if (!rs->fmem) goto fail;

         glxx_lock_fixer_stuff(rs);
         result = create_bin_cl();
         glxx_unlock_fixer_stuff();
         if (!result) goto fail;

         state->changed_cfg = true;             /* We discarded these so flag to */
         state->changed_linewidth = true;       /* recreate the records.         */
         state->changed_polygon_offset = true;
         state->changed_viewport = true;
         state->old_flat_shading_flags = ~0;
         rs->drawn = false;

         useDrawRect = false;
      } else                  /* If we're not clearing everything then we've no choice... */
         useDrawRect = true;
   }
   else
   {
      /* Start of a frame, so choose between h/w clear or draw_rect */
      useDrawRect = !clearingAllBuffers;
   }

   if (useDrawRect)
   {
      INCR_DRIVER_COUNTER(soft_clears);

      result = draw_rect(state, rs, &fb, color, depth, stencil, x, y, xmax, ymax);
   }
   else
   {
      INCR_DRIVER_COUNTER(hard_clears);

      if (color)
      {
         rs->color_buffer_valid = true;
         rs->color_load = false;
         rs->color_value = color_floats_to_rgba(
            state->clear_color[0],
            state->clear_color[1],
            state->clear_color[2],
            state->clear_color[3]);

         if (fb.ms)
            rs->ms_color_buffer_valid = true;
      }
      if (fb.have_depth && depth)
      {
         rs->ds_buffer_valid = true;
         rs->depth_load = false;
         rs->depth_value = state->clear_depth;
      }
      if (fb.have_stencil && stencil)
      {
         rs->ds_buffer_valid = true;
         rs->stencil_load = false;
         rs->stencil_value = (rs->stencil_value & ~state->stencil_mask.front) | (state->clear_stencil & state->stencil_mask.front);
      }
      result = true;
   }

   if (!result) goto fail;

   return true;

fail:
   glxx_hw_discard_frame(rs);
   return false;
}

void glxx_hw_invalidate_internal(GLXX_HW_RENDER_STATE_T *rs, bool color, bool depth, bool stencil, bool multisample)
{
   UNUSED(multisample);

   if (color)
   {
      rs->color_buffer_valid = false;
      rs->color_load = false;
   }
   if ((depth || !rs->installed_fb.have_depth) && (stencil || !rs->installed_fb.have_stencil))
   {
      rs->ds_buffer_valid = false;
   }
   if (multisample || !rs->installed_fb.ms)
   {
      rs->ms_color_buffer_valid = false;
   }
}

bool glxx_hw_start_frame_internal(GLXX_HW_RENDER_STATE_T *rs, GLXX_HW_FRAMEBUFFER_T *fb)
{
   uint32_t tilesize = fb->ms ? (KHRN_HW_TILE_HEIGHT>>1) : KHRN_HW_TILE_HEIGHT;
   bool color_valid_to_load;
   bool aux_storage = false;
   bool aux_valid_to_load = false;
   KHRN_IMAGE_T *color;

   vcos_assert(render_state == NULL);

   rs->xxx_empty = false;
   rs->installed_fb.mh_color_image = MEM_INVALID_HANDLE;
   rs->installed_fb.mh_aux_image = MEM_INVALID_HANDLE;
   rs->hw_frame_count = 0;

   color = (KHRN_IMAGE_T *)mem_lock(fb->mh_color_image);
   color_valid_to_load = !khrn_interlock_is_invalid(&color->interlock);
   khrn_interlock_write(&color->interlock, khrn_interlock_user(rs->name));
   mem_unlock(fb->mh_color_image);

   if(!glxx_lock_fixer_stuff(rs))
      goto quit2;

   render_state->num_tiles_x = (fb->pad_width + tilesize - 1) / tilesize;
   render_state->num_tiles_y = (fb->pad_height + tilesize - 1) / tilesize;

   if (render_state->num_tiles_x == 0 || render_state->num_tiles_y == 0)
      goto quit;

   render_state->installed_fb.ms = fb->ms;

   render_state->fmem = khrn_fmem_init(khrn_interlock_user(render_state->name));
   if (!render_state->fmem)
      goto quit;
   if (!create_bin_cl())
      goto quit;

   MEM_ASSIGN(render_state->installed_fb.mh_color_image, fb->mh_color_image);
   MEM_ASSIGN(render_state->installed_fb.mh_aux_image, fb->mh_aux_image);

   render_state->installed_fb.width = fb->width;
   render_state->installed_fb.height = fb->height;
   render_state->installed_fb.pad_width = fb->pad_width;
   render_state->installed_fb.pad_height = fb->pad_height;
   render_state->installed_fb.col_format = fb->col_format;
   render_state->installed_fb.flags = fb->flags;
   render_state->installed_fb.have_depth = fb->have_depth;
   render_state->installed_fb.have_stencil = fb->have_stencil;
   render_state->installed_fb.dither = fb->dither;
   render_state->installed_fb.stereo_mode = fb->stereo_mode;

   if (fb->mh_aux_image != MEM_INVALID_HANDLE)
   {
      KHRN_IMAGE_T *aux_image = (KHRN_IMAGE_T *)mem_lock(fb->mh_aux_image);
      aux_valid_to_load = !khrn_interlock_is_invalid(&aux_image->interlock);
      khrn_interlock_write(&aux_image->interlock, khrn_interlock_user(rs->name));
      aux_storage = (aux_image->mh_storage != MEM_INVALID_HANDLE);
      mem_unlock(fb->mh_aux_image);
   }

   render_state->color_buffer_valid = true;
   // render_state->ds_buffer_valid = aux_storage; // This is not sensible, since the storage won't be created if the buffer is invalid.
                                                   // There would be a circular dependency between ds_buffer_valid and mh_storage
   render_state->ds_buffer_valid = true;
   render_state->ms_color_buffer_valid = true;
   render_state->color_load = color_valid_to_load;
   render_state->depth_load = aux_valid_to_load && fb->have_depth && aux_storage;
   render_state->stencil_load = aux_valid_to_load && fb->have_stencil && aux_storage;
   render_state->drawn = false;
   render_state->stencil_used = render_state->stencil_load;
#ifndef NDEBUG
   /* useful visual indicator that the color buffer has become undefined */
   render_state->color_value = 0xff0000ff;
#else
   /* This color is pushed out to the tiles when they are undefined.
      This can happen if the user calls eglSwapBuffers() followed by eglSwapBuffers() */
   render_state->color_value = 0;
#endif
   render_state->depth_value = 1.0f;
   render_state->stencil_value = 0;
#ifdef EGL_KHR_fence_sync
   render_state->fence_active = false;
#endif

   if (khrn_workarounds.HW2116)
      render_state->batch_count = 0;

   glxx_unlock_fixer_stuff();

   return true;
quit:
   glxx_unlock_fixer_stuff();
quit2:
   color = (KHRN_IMAGE_T *)mem_lock(fb->mh_color_image);
   khrn_interlock_release(&color->interlock, khrn_interlock_user(rs->name));
   mem_unlock(fb->mh_color_image);
   return false;
}

static bool create_master_cl(void)
{
   KHRN_IMAGE_T *col;
   KHRN_IMAGE_FORMAT_T col_format;
   uint8_t *instr;
   uint32_t offset;
   uint32_t color_value;
   uint8_t pixel_format;

   instr = glxx_big_mem_alloc_cle(41);

   if(!instr)
      goto fail;


   col = (KHRN_IMAGE_T *)mem_lock(render_state->installed_fb.mh_color_image);
   col_format = render_state->installed_fb.col_format & ~(IMAGE_FORMAT_PRE | IMAGE_FORMAT_LIN);

   // Clear colour and depth
   add_byte(&instr, KHRN_HW_INSTR_STATE_CLEARCOL);   //(14)

   if (tu_image_format_rb_swap(khrn_image_to_tf_format(col_format)))  /* TODO: slight hack (and duplicated code) */
   {
      color_value = render_state->color_value;
      color_value = (color_value & 0xff00ff00) | ((color_value & 0xff0000) >> 16) | ((color_value & 0xff) << 16);
   }
   else
   {
      color_value = render_state->color_value;
   }
   add_word(&instr, color_value);
   add_word(&instr, color_value);
   add_word(&instr, (uint32_t)(render_state->depth_value * 16777215.0f));
   add_byte(&instr, render_state->stencil_value);

   add_byte(&instr, KHRN_HW_INSTR_STATE_TILE_RENDERING_MODE);  //(11)

   vcos_assert(col_format == ABGR_8888_RSO ||
          col_format == XBGR_8888_RSO ||
          col_format == ARGB_8888_RSO ||
          col_format == XRGB_8888_RSO ||
          col_format == RGBA_8888_RSO ||
          col_format == RGBX_8888_RSO ||
          col_format == BGRA_8888_RSO ||
          col_format == BGRX_8888_RSO ||
          col_format == RGB_565_RSO   ||
          col_format == ABGR_8888_TF  ||
          col_format == XBGR_8888_TF  ||
          col_format == ARGB_8888_TF  ||
          col_format == XRGB_8888_TF  ||
          col_format == RGBA_8888_TF  ||
          col_format == RGBX_8888_TF  ||
          col_format == RGB_565_TF    ||
          col_format == ABGR_8888_LT  ||
          col_format == XBGR_8888_LT  ||
          col_format == ARGB_8888_LT  ||
          col_format == XRGB_8888_LT  ||
          col_format == RGBA_8888_LT  ||
          col_format == RGBX_8888_LT  ||
          col_format == RGB_565_LT
          );

   if (khrn_workarounds.FB_TOP_DOWN)
   {
      if ((col->flags & IMAGE_FLAG_DISPLAY) && (khrn_image_is_rso(col->format)))
         /* rendering upside down causes the HW to decrement its pointers rather than increment.  Cause this to point to the end of the buffer */
         offset = col->offset + ((col->height - 1) * col->stride);
      else
         offset = col->offset;
   }
   else
      offset = col->offset;

   if (! glxx_big_mem_add_fix(&instr, col->mh_storage, offset)) {
      mem_unlock(render_state->installed_fb.mh_color_image);
      goto fail;
   }
   mem_unlock(render_state->installed_fb.mh_color_image);

   add_short(&instr, render_state->installed_fb.pad_width);   //Width (pixels)

   /* Calculate pixel format (0 = 565 dithered, 1 = 8888, 2 = 565 not dithered) */
   pixel_format = 1<<2;
   if (col_format == RGB_565_RSO || col_format == RGB_565_TF || col_format == RGB_565_LT)
   {
      if (render_state->installed_fb.dither && !khrn_options.force_dither_off)
         pixel_format = 0<<2;
      else
         pixel_format = 2<<2;
   }

   add_short(&instr, render_state->installed_fb.pad_height);  //Height (pixels)
   add_byte(&instr,
      render_state->installed_fb.ms<<0 |                                                                                  /* Multisample mode */
      0<<1 |                                                                                       /* Not 64-bit */
      pixel_format |
      render_state->installed_fb.ms<<4 |                                                                                  /* 1x or 4x decimation */
      (khrn_image_is_tformat(render_state->installed_fb.col_format) ? (1<<6) :
       khrn_image_is_lineartile(render_state->installed_fb.col_format) ? (2<<6) :
       (((khrn_workarounds.FB_TOP_DOWN) && (col->flags & IMAGE_FLAG_DISPLAY) && 
          khrn_image_is_rso(render_state->installed_fb.col_format)) ? (3<<6) : (0<<6)))
      );                                                   /* Memory format */
   add_byte(&instr, 0);     // unused

   /*
      Clear tile buffer
      On 2708A0 this requires a dummy store. On 2708B0 we disable the store.
   */
   add_byte(&instr, KHRN_HW_INSTR_STATE_TILE_COORDS);     //(3)
   add_byte(&instr, 0);
   add_byte(&instr, 0);
   add_byte(&instr, KHRN_HW_INSTR_STORE_GENERAL);         //(7)
   add_short(&instr, 0);                          /* store = none */
   add_word(&instr, 0);                           /* no address needed */

   if (khrn_workarounds.GFXH30)
   {
      /* 2760sim will complain if it gets a primitive with rasosm=1 but tlbms=0.
      * this could happen if we didn't have a STATE_CFG before the first
      * primitive. we don't put STATE_CFGs before the GFXH-30 workaround
      * primitives, so put one here to cover the case where the first primitive is
      * a GFXH-30 workaround primitive */
      add_byte(&instr, KHRN_HW_INSTR_STATE_CFG);             //(4)
      add_byte(&instr, 0); /* rasosm=0 is fine whatever tlbms is */
      add_byte(&instr, 0);
      add_byte(&instr, 0);
   }
   else
   {
      add_byte(&instr, KHRN_HW_INSTR_NOP);
      add_byte(&instr, KHRN_HW_INSTR_NOP);
      add_byte(&instr, KHRN_HW_INSTR_NOP);
      add_byte(&instr, KHRN_HW_INSTR_NOP);
   }

   // Wait for binner to finish before rendering anything
   add_byte(&instr, KHRN_HW_INSTR_WAIT_SEMAPHORE);        //(2)
   add_byte(&instr, KHRN_HW_INSTR_MARKER); // khrn_hw assumes there is a MARKER just after the WAIT_SEMAPHORE

   // Populate the master control list with branch instructions to tile control lists
   if(!populate_master_cl(&render_state->installed_fb))
      goto fail;

   return true;
fail:
   return false;
}

static bool create_bin_cl(void)
{
   uint8_t *instr;
   uint32_t state_size;

   khrn_fmem_start_bin(render_state->fmem);
   instr = glxx_big_mem_alloc_cle(19);
   if(!instr)
      goto fail;
   add_byte(&instr, KHRN_HW_INSTR_STATE_TILE_BINNING_MODE);  //(16)

   state_size = render_state->num_tiles_x * render_state->num_tiles_y * KHRN_HW_TILE_STATE_SIZE;

   if (!glxx_big_mem_add_special(&instr, KHRN_FMEM_SPECIAL_BIN_MEM, 0)) goto fail;
   if (!glxx_big_mem_add_special(&instr, KHRN_FMEM_SPECIAL_BIN_MEM_SIZE, -(int)state_size)) goto fail;
   if (!glxx_big_mem_add_special(&instr, KHRN_FMEM_SPECIAL_BIN_MEM_END, -(int)state_size)) goto fail;

   add_byte(&instr, render_state->num_tiles_x);
   add_byte(&instr, render_state->num_tiles_y);
   add_byte(&instr,
         (render_state->installed_fb.ms ? 1 : 0) |   //Multisample mode
         0<<1   |   //64-bit
         1<<2   |   //Auto-initialise tile state data array
         0<<3   |   //Tile allocation initial block size (32 bytes)
         2<<5);      //Tile allocation block size (32 bytes)
   add_byte(&instr, KHRN_HW_INSTR_START_TILE_BINNING);       //(1)

   /* Ensure primitive format is reset. TODO: is this necessary? */
   add_byte(&instr, KHRN_HW_INSTR_PRIMITIVE_LIST_FORMAT); //(2)
   add_byte(&instr, 0x12);   /* 16 bit triangle */

#ifdef XXX_OFFLINE
   next_user_shader = glxx_fixable_null();
   next_user_unif = glxx_fixable_null();
   next_vcd_addr = glxx_fixable_null();
#endif

   return true;
fail:
   glxx_unlock_fixer_stuff();
   return false;
}

typedef struct {
   bool fence_active;
} HW_CALLBACK_DATA_T;

#ifdef TIMELINE_EVENT_LOGGING
static void EventLog(uint32_t t, uint32_t c, uint32_t r, uint32_t d, char *desc)
{
   EventData ev;
   ev.eventType = t;
   ev.eventCode = c;
   ev.eventRow  = r;
   ev.eventData = d;
   ev.desc = desc;
   khrn_remote_event_log(&ev);
}
#else
#define EventLog(t, c, r, d, desc)
#endif

static void hw_callback(KHRN_HW_CALLBACK_REASON_T reason, void *data, const uint32_t *specials)
{
   HW_CALLBACK_DATA_T *d = (HW_CALLBACK_DATA_T *)data;

   switch (reason) {
   case KHRN_HW_CALLBACK_REASON_FIXUP:
      EventLog(eEVENT_START, eEVENT_BIN, eEVENT_BINNER, 0, 0);
      break;
   case KHRN_HW_CALLBACK_REASON_BIN_FINISHED_LLAT:
      EventLog(eEVENT_END, eEVENT_BIN, eEVENT_BINNER, 0, 0);
      break;
   case KHRN_HW_CALLBACK_REASON_RENDER_SUBMIT:
      EventLog(eEVENT_START, eEVENT_RENDER, eEVENT_RENDERER, 0, 0);
      break;
   case KHRN_HW_CALLBACK_REASON_RENDER_FINISHED_ISR:
      EventLog(eEVENT_END, eEVENT_RENDER, eEVENT_RENDERER, 0, 0);
      break;
   case KHRN_HW_CALLBACK_REASON_OUT_OF_MEM_LLAT:
   case KHRN_HW_CALLBACK_REASON_OUT_OF_MEM:
   case KHRN_HW_CALLBACK_REASON_BIN_FINISHED:
   {
      break;
   }
   case KHRN_HW_CALLBACK_REASON_RENDER_FINISHED:
   {
#if GL_OES_EGL_sync
      if (d->fence_active) {
         /* signal EGL sync object(s) */
         egl_khr_fence_update();
      }
#endif
      break;
   }
   default:
   {
      UNREACHABLE();
   }
   }
}

bool glxx_hw_render_state_flush(GLXX_HW_RENDER_STATE_T *rs)
{
   uint8_t * instr;
   GLXX_HW_FRAMEBUFFER_T *fb;
   KHRN_IMAGE_T *color, *aux_image;
   GLXX_HW_RENDER_STATE_T *stashed_rs = NULL;
   HW_CALLBACK_DATA_T *callback_data;

   vcos_assert(!rs->xxx_empty);

#ifdef DRAW_TEX_LOGGING
   printf("render_state_flush: rs: %d\n",rs->name);
   printf("--------------------\n");
#endif

   /* Interlock transfer delayed until we know that we can't fail */

   /*
   This function can be called in the middle of doing other fixer stuff, so need to
   push and pop static render state
   */
   if (render_state)
   {
      stashed_rs = render_state;
      glxx_unlock_fixer_stuff();
   }

   if(!vcos_verify(glxx_lock_fixer_stuff(rs)))//lock fixer stuff can only fail in start_frame
      goto quit2;

   // Terminate tile control lists if necessary
   instr = glxx_big_mem_alloc_cle(2);
   if(!instr)
      goto quit;

   add_byte(&instr, KHRN_HW_INSTR_INCR_SEMAPHORE);   //(1)
   add_byte(&instr, KHRN_HW_INSTR_FLUSH);

   if (!khrn_fmem_start_render(render_state->fmem))
      goto quit;

   if (!create_master_cl())
      goto quit;

   fb = &render_state->installed_fb;

   glxx_unlock_fixer_stuff();

   /* Now transfer everything */
   color = (KHRN_IMAGE_T *)mem_lock(rs->installed_fb.mh_color_image);
   khrn_hw_advance_enter_pos();
   khrn_interlock_transfer(&color->interlock, khrn_interlock_user(rs->name), KHRN_INTERLOCK_FIFO_HW_RENDER);
   mem_unlock(rs->installed_fb.mh_color_image);

   if(rs->installed_fb.mh_aux_image != MEM_INVALID_HANDLE)
   {
      aux_image = (KHRN_IMAGE_T *)mem_lock(rs->installed_fb.mh_aux_image);
      khrn_interlock_transfer(&aux_image->interlock, khrn_interlock_user(rs->name), KHRN_INTERLOCK_FIFO_HW_RENDER);
      mem_unlock(rs->installed_fb.mh_aux_image);
   }

   callback_data = khrn_fmem_queue(
      rs->fmem,
      /* todo: we probably won't need to clear the tu caches for binning most of the time */
      (KHRN_HW_CC_FLAG_T)(KHRN_HW_CC_FLAG_L2 | KHRN_HW_CC_FLAG_IC | KHRN_HW_CC_FLAG_UC | KHRN_HW_CC_FLAG_TU),
      (KHRN_HW_CC_FLAG_T)(KHRN_HW_CC_FLAG_L2 | KHRN_HW_CC_FLAG_IC | KHRN_HW_CC_FLAG_UC | KHRN_HW_CC_FLAG_TU),
      rs->hw_frame_count,
      0,
      rs->num_tiles_x * rs->num_tiles_y * (KHRN_HW_CL_BLOCK_SIZE_MIN + KHRN_HW_TILE_STATE_SIZE),
      0, KHRN_HW_VPM_BLOCKS_N - 30,
      KHRN_HW_TYPE_GL,
      hw_callback,
      sizeof(HW_CALLBACK_DATA_T));

#ifdef EGL_KHR_fence_sync      
   callback_data->fence_active = rs->fence_active;
#endif
   khrn_hw_ready(true, callback_data);

#ifdef ABORT_AFTER_FIRST_FRAME
   /* HACK HACK HACK */
      exit(0);
#endif

   MEM_ASSIGN(rs->installed_fb.mh_color_image, MEM_INVALID_HANDLE);
   MEM_ASSIGN(rs->installed_fb.mh_aux_image, MEM_INVALID_HANDLE);
   khrn_render_state_finish(rs->name);
   if (stashed_rs) glxx_lock_fixer_stuff(stashed_rs);

   return true;

quit:
   glxx_unlock_fixer_stuff();
quit2:
   glxx_hw_discard_frame(rs);
   if (stashed_rs) glxx_lock_fixer_stuff(stashed_rs);
   return false;
}


/*!
 * \brief Sets up the master control list by inserting the branches to all the per-tile lists.
 * Also inserts a tile coordinate instruction before each one and a store subsample afterwards.
 */
static bool populate_master_cl(GLXX_HW_FRAMEBUFFER_T *fb)
{
   uint32_t y;
   uint8_t *instr;
   bool load_full_color, load_full_depth, store_full_color, store_full_depth;
   bool load_standard, load_full, store_full, stereo_mode;
   uint32_t alloc_tile, alloc;
   MEM_HANDLE_T full_handle = MEM_INVALID_HANDLE;
   uint32_t full_stride = 0, full_load_offset = 0, full_store_offset = 0;
   bool alloc_by_row = false;
   uint32_t alloc_row;
   KHRN_IMAGE_T *aux_image = NULL;
   KHRN_IMAGE_T *color_image;
   uint32_t gfxh30_counter = 0;
   uint32_t *gfxh30_shader_rec;
   uint32_t *gfxh30_vertices;

   if (fb->mh_aux_image != MEM_INVALID_HANDLE)
      aux_image = (KHRN_IMAGE_T *)mem_lock(fb->mh_aux_image);

   color_image = (KHRN_IMAGE_T *)mem_lock(fb->mh_color_image);

   load_standard = render_state->color_load && !fb->ms;
   load_full_color = fb->ms && render_state->color_load;
   load_full_depth = render_state->depth_load || render_state->stencil_load;
   store_full_color = fb->ms && render_state->ms_color_buffer_valid;
   store_full_depth = (fb->have_depth || fb->have_stencil) && render_state->ds_buffer_valid;

   load_full = load_full_color || load_full_depth;
   store_full = store_full_color || store_full_depth;

   stereo_mode = fb->stereo_mode;

   if (load_full || store_full)
   {
      vcos_assert(aux_image != NULL);
      vcos_assert(aux_image->format == DEPTH_32_TLBD || aux_image->format == DEPTH_COL_64_TLBD || aux_image->format == COL_32_TLBD);
      full_stride = khrn_image_get_bpp(aux_image->format) * (KHRN_HW_TILE_HEIGHT * KHRN_HW_TILE_WIDTH / 8);
      full_load_offset = aux_image->offset;
      full_store_offset = aux_image->offset;

      /* Skip over color when loading/storing if necessary */
      if (!load_full_color && aux_image->format == DEPTH_COL_64_TLBD) full_load_offset += KHRN_HW_TILE_HEIGHT * KHRN_HW_TILE_WIDTH * 4;
      if (!store_full_color && aux_image->format == DEPTH_COL_64_TLBD) full_store_offset += KHRN_HW_TILE_HEIGHT * KHRN_HW_TILE_WIDTH * 4;

      /* Allocate some memory if necessary. TODO: this code should probably be in khrn_image */
      if (!khrn_image_alloc_storage(aux_image, "KHRN_IMAGE_T.storage OpenGL ES aux buffer"))
         return false;


      full_handle = aux_image->mh_storage;
   }
   vcos_assert(color_image->mh_storage != MEM_INVALID_HANDLE);

   INCR_DRIVER_COUNTER(tb_grp_color_stores);

   /* Decide how much room we need for an instruction */
   alloc_tile = 11;

   if (load_standard) 
   {
      alloc_tile += 7;
      INCR_DRIVER_COUNTER(tb_grp_color_loads);
   }
   if (load_full) 
   {
      alloc_tile += 5;
      if (load_full_color)
         INCR_DRIVER_COUNTER(tb_grp_ms_color_loads);
      if (load_full_depth)
         INCR_DRIVER_COUNTER(tb_grp_ds_loads);
   }
   if (load_standard && load_full) 
   {
      alloc_tile += 10;
   }
   if (store_full) 
   {
      alloc_tile += 8;
      if (store_full_color)
         INCR_DRIVER_COUNTER(tb_grp_ms_color_stores);
      if (store_full_depth)
         INCR_DRIVER_COUNTER(tb_grp_ds_stores);
   }

   alloc_row = alloc_tile * render_state->num_tiles_x;
   if (!khrn_workarounds.GFXH30)
   {
      if(alloc_row < 2048)
         alloc_by_row = true;
   }
   else
   {
      gfxh30_vertices = glxx_big_mem_alloc_junk(36, 4);
      if (!gfxh30_vertices) goto fail;
      gfxh30_vertices[0] = 0xff00ff00;
      gfxh30_vertices[1] = 0;
      gfxh30_vertices[2] = 0;
      gfxh30_vertices[3] = 0xff00ff00;
      gfxh30_vertices[4] = 0;
      gfxh30_vertices[5] = 0;
      gfxh30_vertices[6] = 0xff00ff01;
      gfxh30_vertices[7] = 0;
      gfxh30_vertices[8] = 0;


      gfxh30_shader_rec = glxx_big_mem_alloc_junk(16, 16);
      if (!gfxh30_shader_rec) goto fail;
      gfxh30_shader_rec[0] = 12 << 8;
      gfxh30_shader_rec[1] = 0;
      gfxh30_shader_rec[2] = 0;
      gfxh30_shader_rec[3] = khrn_hw_addr(khrn_hw_alias_direct(gfxh30_vertices));
   }

   /*
      Insert a series of:
      - tile coords
      - branch instructions
      - store subsample instructions
      into the master control list.
   */
   for (y = 0; y < render_state->num_tiles_y; y++) {
      unsigned int x_original;
      if(alloc_by_row) {
         alloc = alloc_row;
         instr = glxx_big_mem_alloc_cle(alloc);
         if(!instr)
            goto fail;
      }
      for (x_original = 0; x_original < render_state->num_tiles_x; x_original++) {
         unsigned int x;
         if (stereo_mode)
         {
            if (x_original & 0x1)
               x = (x_original >> 1) + ((render_state->num_tiles_x + 1) / 2);
            else
               x = (x_original >> 1);
         }
         else
            x = x_original;

         if(!alloc_by_row) {
            alloc = alloc_tile;
            if (khrn_workarounds.GFXH30)
            {
               gfxh30_counter++;
               if (gfxh30_counter == 5)
               {
                  gfxh30_counter = 0;
                  alloc += 15;
               }
            }
            instr = glxx_big_mem_alloc_cle(alloc);
            if(!instr)
               goto fail;
         }

         if (load_standard)
         {
            uint16_t flags;
            uint32_t offset;

            INCR_DRIVER_COUNTER(tb_color_loads);

            add_byte(&instr, KHRN_HW_INSTR_LOAD_GENERAL);          /*(7) */

            flags = 1;      /* load = colour */
            if (khrn_image_is_tformat(fb->col_format)) 
               flags |= (1 << 4);
            else if (khrn_image_is_lineartile(fb->col_format)) 
               flags |= (2 << 4);
            else if ((khrn_workarounds.FB_TOP_DOWN) && (fb->flags & IMAGE_FLAG_DISPLAY) && khrn_image_is_rso(fb->col_format)) 
               flags |= (3 << 4);

            switch (khrn_image_to_rso_format(fb->col_format) & ~(IMAGE_FORMAT_PRE | IMAGE_FORMAT_LIN))
            {
            case ABGR_8888_RSO:
            case XBGR_8888_RSO:
            case RGBA_8888_RSO:
            case RGBX_8888_RSO:
            case BGRA_8888_RSO:
            case BGRX_8888_RSO:
            case ARGB_8888_RSO:
            case XRGB_8888_RSO: flags |= 0<<8; break;
            case RGB_565_RSO: flags |= 2<<8; break;
            default: UNREACHABLE();
            }
            add_short(&instr, flags);

            if (khrn_workarounds.FB_TOP_DOWN)
            {
               if ((fb->flags & IMAGE_FLAG_DISPLAY) && (khrn_image_is_rso(color_image->format)))
                  /* rendering upside down causes the HW to decrement its pointers rather than increment.  Cause this to point to the end of the buffer */
                  offset = color_image->offset + ((color_image->height - 1) * color_image->stride);
               else
                  offset = color_image->offset;
            }
            else
            {
               offset = color_image->offset;
            }

            if (! glxx_big_mem_add_fix(&instr, color_image->mh_storage, offset) ) return false;
         }

         if (load_standard && load_full)
         {
            add_byte(&instr, KHRN_HW_INSTR_STORE_GENERAL);          //(7)
            add_short(&instr, 0 | 1 << 13 | 1 << 14);      /* store = none. disable colour & depth clear */
            add_word(&instr, 0);                           /* no address needed */
            add_byte(&instr, KHRN_HW_INSTR_STATE_TILE_COORDS);      //(3)
            add_byte(&instr, x);
            add_byte(&instr, y);
         }

         if (load_full)
         {
            uint32_t flags;

            if (load_full_color)
               INCR_DRIVER_COUNTER(tb_ms_color_loads);

            if (load_full_depth)
               INCR_DRIVER_COUNTER(tb_ds_loads);

            vcos_assert(full_load_offset + full_stride * (y * render_state->num_tiles_x + x) < mem_get_size(full_handle));

            add_byte(&instr, KHRN_HW_INSTR_LOAD_FULL);          //(5)

            flags = 0;
            if (!load_full_color || !fb->ms) flags |= 1<<0; /* disable colour read */
            if (!load_full_depth) flags |= 1<<1;            /* disable depth read */

            if (! glxx_big_mem_add_fix(&instr,
                        full_handle,
                        full_load_offset + full_stride * (y * render_state->num_tiles_x + x) + flags) )
            {
               return false;
            }
         }

         add_byte(&instr, KHRN_HW_INSTR_STATE_TILE_COORDS);   //(3)
         add_byte(&instr, x);
         add_byte(&instr, y);

         /* Ensure primitive format is reset. TODO: is this necessary? */
         add_byte(&instr, KHRN_HW_INSTR_PRIMITIVE_LIST_FORMAT); //(2)
         add_byte(&instr, 0x12);   /* 16 bit triangle */

         if (khrn_workarounds.GFXH30)
         {
            if (gfxh30_counter == 0)
            {
               add_byte(&instr, KHRN_HW_INSTR_NV_SHADER);    //(5; dummy)
               add_pointer(&instr, gfxh30_shader_rec);
               add_byte(&instr, KHRN_HW_INSTR_GLDRAWARRAYS); //(10; dummy)
               add_byte(&instr, 1<<4 | 4);     /* triangles */
               add_word(&instr, 3);
               add_word(&instr, 0);
            }
         }

         add_byte(&instr, KHRN_HW_INSTR_BRANCH_SUB);          //(5)

         if (! glxx_big_mem_add_special(&instr,
                     KHRN_FMEM_SPECIAL_BIN_MEM,
                     (x + y * render_state->num_tiles_x) * KHRN_HW_CL_BLOCK_SIZE_MIN) )
         {
            return false;
         }

         if (store_full)
         {
            uint32_t flags;

            if (store_full_color)
               INCR_DRIVER_COUNTER(tb_ms_color_stores);
            if (store_full_depth)
               INCR_DRIVER_COUNTER(tb_ds_stores);

            vcos_assert(full_store_offset + full_stride * (y * render_state->num_tiles_x + x) < mem_get_size(full_handle));

            flags = 1<<2;                               /* disable clear. Not last tile in frame. */
            if (!store_full_color || !fb->ms) flags |= 1<<0; /* disable colour write */
            if (!store_full_depth) flags |= 1<<1;            /* disable depth write */

            add_byte(&instr, KHRN_HW_INSTR_STORE_FULL);          //(5)

            if (! glxx_big_mem_add_fix(&instr,
                        full_handle,
                        full_store_offset + full_stride * (y * render_state->num_tiles_x + x) + flags) )
            {
               return false;
            }

            add_byte(&instr, KHRN_HW_INSTR_STATE_TILE_COORDS);   //(3)
            add_byte(&instr, x);
            add_byte(&instr, y);
         }

         if (x == render_state->num_tiles_x - 1 && y == render_state->num_tiles_y - 1) {
            add_byte(&instr, KHRN_HW_INSTR_STORE_SUBSAMPLE_EOF);     // (1). Last tile needs special store instruction
            render_state->hw_frame_count++;
         }
         else
         {
            add_byte(&instr, KHRN_HW_INSTR_STORE_SUBSAMPLE);
         }

         INCR_DRIVER_COUNTER(tb_color_stores);
      }
   }

   if (fb->mh_aux_image != MEM_INVALID_HANDLE)
     mem_unlock(fb->mh_aux_image);
   mem_unlock(fb->mh_color_image);

   return true;

fail:

   if (fb->mh_aux_image != MEM_INVALID_HANDLE)
     mem_unlock(fb->mh_aux_image);
   mem_unlock(fb->mh_color_image);

   return false;
}

static bool draw_rect(
   GLXX_SERVER_STATE_T *state,
   GLXX_HW_RENDER_STATE_T *rs,
   GLXX_HW_FRAMEBUFFER_T *fb,
   bool color, bool depth, bool stencil,
   int x, int y, int xmax, int ymax)
{
   uint8_t *instr;
   uint32_t *fshader;
   uint32_t *funif;
   uint32_t *vdata;
   uint32_t *rec;
   uint32_t *locked_addr;
   uint32_t z;
   uint32_t selector;
   uint32_t *shaderCode;
   uint32_t uniformCount;
   uint32_t uniformIndx;
   bool egl_output;

   if(!vcos_verify(glxx_lock_fixer_stuff(rs)))//lock fixer stuff can only fail in start_frame
      return false;

   render_state->drawn = true;
   render_state->color_buffer_valid = true;
   if (fb->have_depth || fb->have_stencil)
      render_state->ds_buffer_valid = true;
   if (fb->ms)
      render_state->ms_color_buffer_valid = true;

   if (khrn_workarounds.FB_BOTTOM_UP)
   {
      /* work off if the output image is an EGL image.  This can be a wrapped image, or an eglSurface or a Pixmap */
      /* used to only trigger of RSO, but make it work from any egl image irrespective of format */
      egl_output = (fb->flags & IMAGE_FLAG_DISPLAY) ? true : false;
   }
   else if (khrn_workarounds.FB_TOP_DOWN)
   {
      /* on a B1 we can't rasterize TFormat upside down in HW, so we need to import the original flip from B0 */
      egl_output = ((fb->flags & IMAGE_FLAG_DISPLAY) &&
                  (khrn_image_is_tformat(fb->col_format) || khrn_image_is_lineartile(fb->col_format))) ? true : false;
   }
   else
      egl_output = false;

   // Set up control list
   instr = glxx_big_mem_alloc_cle(34);
   if(!instr)
      goto fail;

   /*
    * Emit scissor/clipper/viewport instructions
    */
   add_byte(&instr, KHRN_HW_INSTR_STATE_CLIP);     //(9)
   add_short(&instr, x);
   if ((khrn_workarounds.FB_BOTTOM_UP || khrn_workarounds.FB_TOP_DOWN) && egl_output)
      add_short(&instr, fb->height - ymax);
   else
      add_short(&instr, y);
   add_short(&instr, xmax - x);
   add_short(&instr, ymax - y);
   state->changed_viewport = true;  /* Clear and render might end up with different clip rectangles - clear doesn't clip to viewport */

   /*
    * Emit a Configuration record
    */
   add_byte(&instr, KHRN_HW_INSTR_STATE_CFG);     /*(4) */
   if ((khrn_workarounds.FB_BOTTOM_UP || khrn_workarounds.FB_TOP_DOWN) && egl_output)
      add_byte(&instr, 1 | 1<<1 | 1<<2);   /* enfwd, enrev, cwise */
   else
      add_byte(&instr, 1 | 1<<1);        /*enfwd, enrev */

   if (depth)
      add_byte(&instr, 7 << 4 | 1 << 7); /* zfunc=always, enzu */
   else if (stencil)
      add_byte(&instr, 7 << 4);          /* zfunc=always, !enzu */
   else
      add_byte(&instr, 0);               /* zfunc=never, !enzu */

   add_byte(&instr, 1<<1);            /* not enez, enezu */

   state->changed_cfg = true;       /* Clear and render probably use different configs */

   add_byte(&instr, KHRN_HW_INSTR_STATE_VIEWPORT_OFFSET);  //(5)
   add_short(&instr, 0);
   add_short(&instr, 0);

   //TODO: other miscellaneous pieces of state

   /* Select the correct clear shader */
   selector = 0;
   if (color)
      selector |= CLR_COLOR;
   if (depth)
      selector |= CLR_DEPTH;
   if (stencil)
      selector |= CLR_STENCIL;

   if (color && state->shader.common.blend.color_mask != 0xFFFFFFFF)
      selector |= CLR_MASKED;

   if (fb->ms)
      selector |= CLR_MULTISAMPLE;

   shaderCode = clear_shaders[selector];
   vcos_assert(shaderCode);

   /* How many uniforms will we have? */
   uniformCount = 0;
   if (stencil || depth)
      uniformCount += 2;
   if (color)
      uniformCount++;
   if (color && state->shader.common.blend.color_mask != 0xFFFFFFFF)
      uniformCount++;

   locked_addr = glxx_big_mem_alloc_junk(ALIGN_UP(clear_shader_sizes[selector], 16) + (ALIGN_UP(uniformCount, 4) * 4) + 48 + 16, 16);
   if (!locked_addr)
      goto fail;

   fshader = locked_addr;

   khrn_memcpy(locked_addr, shaderCode, clear_shader_sizes[selector]);

   locked_addr += ALIGN_UP(clear_shader_sizes[selector], 16)/4;
   funif = locked_addr;

   uniformIndx = 0;

   /* First uniform is always color mask if needed */
   if (color && state->shader.common.blend.color_mask != 0xFFFFFFFF)
      ((uint32_t *)locked_addr)[uniformIndx++] = state->shader.common.blend.color_mask;

   /* Set the stencil mode uniform (if needed) */
   if (stencil || depth)
   {
      if (stencil)
      {
         ((uint32_t *)locked_addr)[uniformIndx++] = stencil_clear_mode(state->clear_stencil);
         ((uint32_t *)locked_addr)[uniformIndx++] = stencil_mask_mode(state->stencil_mask.front);
      }
      else
      {
         ((uint32_t *)locked_addr)[uniformIndx++] = stencil_keep_mode;
         ((uint32_t *)locked_addr)[uniformIndx++] = stencil_keep_mode;
      }
   }

   /* Set the colour uniform (if needed) */
   if (color)
   {
      ((uint32_t *)locked_addr)[uniformIndx++] = color_floats_to_rgba(
         state->clear_color[0],
         state->clear_color[1],
         state->clear_color[2],
         state->clear_color[3]);
   }

   locked_addr += ALIGN_UP(uniformCount, 4);
   vdata = locked_addr;

   z = float_to_bits(state->clear_depth);
   if ((khrn_workarounds.FB_BOTTOM_UP || khrn_workarounds.FB_TOP_DOWN) && egl_output)
      vdata[0] = (fb->height - y) << 20 | x << 4;
   else
      vdata[0] = y << 20 | x << 4;
   vdata[1] = z;
   vdata[2] = 0x3f800000;
   if ((khrn_workarounds.FB_BOTTOM_UP || khrn_workarounds.FB_TOP_DOWN) && egl_output)
      vdata[3] = (fb->height - y) << 20 | xmax << 4;
   else
      vdata[3] = y << 20 | xmax << 4;
   vdata[4] = z;
   vdata[5] = 0x3f800000;
   if ((khrn_workarounds.FB_BOTTOM_UP || khrn_workarounds.FB_TOP_DOWN) && egl_output)
      vdata[6] = (fb->height - ymax) << 20 | xmax << 4;
   else
      vdata[6] = ymax << 20 | xmax << 4;
   vdata[7] = z;
   vdata[8] = 0x3f800000;
   if ((khrn_workarounds.FB_BOTTOM_UP || khrn_workarounds.FB_TOP_DOWN) && egl_output)
      vdata[9] = (fb->height - ymax) << 20 | x << 4;
   else
      vdata[9] = ymax << 20 | x << 4;
   vdata[10] = z;
   vdata[11] = 0x3f800000;

   //TODO: stencil, masked color

   locked_addr += 12;
   rec = locked_addr;

   rec[0] = 1 | 12 << 8;  //flags: single-threaded. No point size, clipping, clip header. vstride=12
   rec[1] = khrn_hw_addr(khrn_hw_alias_direct(fshader)); //PTR
   rec[2] = khrn_hw_addr(khrn_hw_alias_direct(funif));   //PTR
   rec[3] = khrn_hw_addr(khrn_hw_alias_direct(vdata));   //PTR
   locked_addr += 4;

   add_byte(&instr, KHRN_HW_INSTR_NV_SHADER);     //(5)
   add_pointer(&instr, rec);

   /*
    * Now insert a GLDrawElements or GLDrawArrays record
    */

   // Emit a GLDRAWARRAYS instruction
   add_byte(&instr, KHRN_HW_INSTR_GLDRAWARRAYS);
   add_byte(&instr, 6);                         //Primitive mode (triangle_fan)
   add_word(&instr, 4);                         //Length (number of vertices)
   add_word(&instr, 0);                         //Index of first vertex

   add_byte(&instr, KHRN_HW_INSTR_NOP);        //(1) TODO: is this necessary?

   glxx_unlock_fixer_stuff();
   return true;

fail:
   glxx_unlock_fixer_stuff();
   return false;
}

uint32_t glxx_hw_convert_operation(GLenum operation)
{
   switch (operation) {
   case GL_ZERO:
      return 0;
   case GL_KEEP:
      return 1;
   case GL_REPLACE:
      return 2;
   case GL_INCR:
      return 3;
   case GL_DECR:
      return 4;
   case GL_INVERT:
      return 5;
   case GL_INCR_WRAP:
      return 6;
   case GL_DECR_WRAP:
      return 7;
   default:
      UNREACHABLE();
      return 0;
   }
}

uint32_t glxx_hw_convert_test_function(GLenum function)
{
   vcos_assert(function >= GL_NEVER && function < GL_NEVER + 8);
   return function - GL_NEVER;
}


bool glxx_lock_fixer_stuff(GLXX_HW_RENDER_STATE_T *rs)
{
   vcos_assert(render_state == NULL);
   render_state = rs;
   return true;
}

void glxx_unlock_fixer_stuff()
{
   vcos_assert(render_state != NULL);
   render_state = NULL;
}


bool glxx_hw_texture_fix(MEM_HANDLE_T thandle)
{
   bool result = true;
   GLXX_TEXTURE_T *texture;

   if (thandle == MEM_INVALID_HANDLE)
      return true;

   vcos_assert(render_state != NULL);
   texture = (GLXX_TEXTURE_T *)mem_lock(thandle);
   khrn_interlock_read((KHRN_INTERLOCK_T *)((char *)texture + glxx_texture_get_interlock_offset(texture)), khrn_interlock_user(render_state->name));
   result &= glxx_hw_insert_interlock(thandle, glxx_texture_get_interlock_offset(texture));

   if (texture->explicit_mipmaps) {
      int min_buffer = 0, max_buffer = 0, i, j;

      switch (texture->target) {
      case GL_TEXTURE_2D:
         min_buffer = TEXTURE_BUFFER_TWOD;
         max_buffer = TEXTURE_BUFFER_TWOD;
         break;
      case GL_TEXTURE_CUBE_MAP:
         min_buffer = TEXTURE_BUFFER_POSITIVE_X;
         max_buffer = TEXTURE_BUFFER_NEGATIVE_Z;
         break;
      case GL_TEXTURE_EXTERNAL_OES:
         min_buffer = TEXTURE_BUFFER_EXTERNAL;
         max_buffer = TEXTURE_BUFFER_EXTERNAL;
         break;
      default:
         UNREACHABLE();
      }
      for (i = min_buffer; i <= max_buffer; i++) {
         for (j = 0; j <= LOG2_MAX_TEXTURE_SIZE; j++) {
            if (texture->mh_mipmaps[i][j] != MEM_INVALID_HANDLE) {
               khrn_interlock_read(&((KHRN_IMAGE_T *)mem_lock(texture->mh_mipmaps[i][j]))->interlock, khrn_interlock_user(render_state->name));
               mem_unlock(texture->mh_mipmaps[i][j]);
               result &= glxx_hw_insert_interlock(texture->mh_mipmaps[i][j], offsetof(KHRN_IMAGE_T, interlock));
            }
         }
      }
   }
   mem_unlock(thandle);
   return result;
}

void glxx_hw_discard_frame(GLXX_HW_RENDER_STATE_T *rs)
{
   //do the tidying up that glxx_hw_flush and hw_callback do
   //but without flushing to the hardware

   if (rs->installed_fb.mh_color_image != MEM_INVALID_HANDLE)
   {
      KHRN_IMAGE_T *color = (KHRN_IMAGE_T *)mem_lock(rs->installed_fb.mh_color_image);

      //release the interlock rather than transferring it
      khrn_interlock_release(&color->interlock, khrn_interlock_user(rs->name));
      mem_unlock(rs->installed_fb.mh_color_image);
   }

   if (rs->installed_fb.mh_aux_image != MEM_INVALID_HANDLE)
   {
      KHRN_IMAGE_T *aux_image = (KHRN_IMAGE_T *)mem_lock(rs->installed_fb.mh_aux_image);

      //release the interlock rather than transferring it
      khrn_interlock_release(&aux_image->interlock, khrn_interlock_user(rs->name));
      mem_unlock(rs->installed_fb.mh_aux_image);
   }

   /*junk_mem = rs->current_junk_mem;
   while (junk_mem != MEM_INVALID_HANDLE)
   {
      MEM_HANDLE_T next_junk_mem;
      next_junk_mem = *(MEM_HANDLE_T *)mem_lock(junk_mem);
      mem_unlock(junk_mem);
      free_junk_mem(junk_mem);
      junk_mem = next_junk_mem;
   }

   rs->current_junk_mem = MEM_INVALID_HANDLE;
   rs->current_junk_offset = 0;*/

   rs->hw_frame_count = 0;

   if (rs->fmem) khrn_fmem_discard(rs->fmem);

   MEM_ASSIGN(rs->installed_fb.mh_color_image, MEM_INVALID_HANDLE);
   MEM_ASSIGN(rs->installed_fb.mh_aux_image, MEM_INVALID_HANDLE);

#if GL_OES_EGL_sync
   if (rs->fence_active) {
      /* signal EGL sync object(s) */
      egl_khr_fence_update();
      rs->fence_active = false;
   }
#endif

   khrn_render_state_finish(rs->name);
}

uint32_t glxx_render_state_name(void)
{
   vcos_assert(render_state != NULL);
   return render_state->name;
}


uint8_t *glxx_big_mem_alloc_cle(int size)
{
   vcos_assert(render_state != NULL);
   return khrn_fmem_cle(render_state->fmem, size);
}

uint32_t *glxx_big_mem_alloc_junk(int size, int align)
{
   vcos_assert(render_state != NULL);
   return khrn_fmem_junk(render_state->fmem, size, align);
}

bool glxx_big_mem_insert(uint32_t *location, MEM_HANDLE_T handle, uint32_t offset)
{
   vcos_assert(render_state != NULL);
   return khrn_fmem_fix(render_state->fmem, location, handle, offset);
}

bool glxx_big_mem_add_fix(uint8_t **p, MEM_HANDLE_T handle, uint32_t offset)
{
   vcos_assert(render_state != NULL);
   return khrn_fmem_add_fix(render_state->fmem, p, handle, offset);
}

bool glxx_big_mem_add_special(uint8_t **p, uint32_t special_i, uint32_t offset)
{
   vcos_assert(render_state != NULL);
   return khrn_fmem_add_special(render_state->fmem, p, special_i, offset);
}

bool glxx_hw_insert_interlock(MEM_HANDLE_T handle, uint32_t offset)
{
   vcos_assert(render_state != NULL);
   return khrn_fmem_interlock(render_state->fmem, handle, offset);
}
