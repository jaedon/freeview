/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Header file

FILE DESCRIPTION
EGL server-side state structure declaration.
=============================================================================*/

#ifndef EGL_SERVER_H
#define EGL_SERVER_H

#include "interface/khronos/egl/egl_int.h"
#include "middleware/khronos/common/khrn_map.h"
#include "middleware/khronos/common/khrn_image.h"
#include "middleware/khronos/common/khrn_hw.h"
#include "middleware/khronos/egl/egl_disp.h"
#include "interface/khronos/include/EGL/egl.h"
#include "interface/khronos/include/EGL/eglext.h"
#include "middleware/khronos/vg/vg_image.h"

// Must be enough for triple-buffering (windows) and mipmaps (pbuffers)
#define EGL_MAX_BUFFERS       12

// There is a single global instance of this
typedef struct
{
   KHRN_MAP_T surfaces;
   KHRN_MAP_T glcontexts;
   KHRN_MAP_T vgcontexts;
   KHRN_MAP_T eglimages;
#if EGL_KHR_sync
   KHRN_MAP_T syncs;
#endif

   uint32_t next_surface;
   uint32_t next_context;
   uint32_t next_eglimage;
#if EGL_KHR_sync
   uint32_t next_sync;
#endif

   uint64_t pid;                   //currently selected process id

   uint32_t glversion;             //EGL_SERVER_GL11 or EGL_SERVER_GL20. (0 if invalid)
   MEM_HANDLE_T glcontext;
   MEM_HANDLE_T gldrawsurface;     //EGL_SERVER_SURFACE_T
   MEM_HANDLE_T glreadsurface;     //EGL_SERVER_SURFACE_T
   MEM_HANDLE_T vgcontext;
   MEM_HANDLE_T vgsurface;         //EGL_SERVER_SURFACE_T

   /*
      locked_glcontext

      Invariants:

      (EGL_SERVER_STATE_LOCKED_GLCONTEXT)
      locked_glcontext == NULL or locked_glcontext is the locked version of glcontext (and we own the lock)
   */
   void *locked_glcontext;
   /*
      locked_vgcontext

      Invariants:

      (EGL_SERVER_STATE_LOCKED_VGCONTEXT)
      locked_vgcontext == NULL or locked_vgcontext is the locked version of vgcontext (and we own the lock)
   */
   void *locked_vgcontext;
   /*
      locked_vgcontext_shared

      Invariants:

      (EGL_SERVER_STATE_LOCKED_VGCONTEXT_SHARED)
      locked_vgcontext_shared == NULL or locked_vgcontext_shared is the locked version of vgcontext->shared_state (and we own the lock)
   */
   void *locked_vgcontext_shared;
   /*
      locked_vgcontext_shared_objects_storage

      Invariants:

      (EGL_SERVER_STATE_LOCKED_VGCONTEXT_SHARED_OBJECTS_STORAGE)
      locked_vgcontext_shared_objects_storage == NULL or locked_vgcontext_shared_objects_storage is the locked version of vgcontext->shared_state->objects->storage (and we own the lock)
   */
   void *locked_vgcontext_shared_objects_storage;

#if EGL_BRCM_driver_monitor
   uint32_t driver_monitor_refcount;
   KHRN_DRIVER_COUNTERS_T driver_monitor_counters;
#endif
} EGL_SERVER_STATE_T;

typedef struct
{
   uint32_t name;

   bool mipmap;
   uint32_t buffers;
   uint32_t back_buffer_index;
   /*
      mh_color

      Invariant:

      For 0 <= i < buffers
         mh_color[i] is a handle to a valid KHRN_IMAGE_T
   */
   MEM_HANDLE_T mh_color[EGL_MAX_BUFFERS];
   MEM_HANDLE_T mh_depth;  //floating KHRN_IMAGE_T
   MEM_HANDLE_T mh_multi;  //floating KHRN_IMAGE_T
   MEM_HANDLE_T mh_mask;   //floating KHRN_IMAGE_T

   uint8_t config_depth_bits;   // How many depth bits were requested in config. May not match actual buffer.
   uint8_t config_stencil_bits; // How many stencil bits were requested in config. May not match actual buffer.

   uint32_t win;                    // Opaque handle passed to egl_server_platform_display
   uint32_t swapchainc;             // from the platform, tells the driver which mode its running in
   uint64_t pid;                    // Opaque handle to creating process
   uint32_t sem;                    // Opaque handle (probably semaphore name) passed on KHAN channel

   MEM_HANDLE_T mh_bound_texture;
   uint32_t swap_interval;
   uint32_t semaphoreId;  //Symbian needs a handle passed back in Khan, not just surface number

   EGL_DISP_HANDLE_T disp_handle;
} EGL_SERVER_SURFACE_T;

typedef struct
{
   int32_t type;
   uint32_t condition;
   int32_t threshold;

   uint64_t pid;
   uint32_t sem;

   bool state;
} EGL_SERVER_SYNC_T;

extern void egl_server_shutdown(void);

extern EGL_SERVER_STATE_T egl_server_state;

/*
   EGL_SERVER_STATE_T *EGL_GET_SERVER_STATE()

   Returns pointer to EGL server state.

   Implementation notes:

   There is only one of these globally, and it does not need locking and unlocking.

   Preconditions:

   Valid EGL server state exists

   Postconditions:

   Return value is a valid pointer
*/

static INLINE EGL_SERVER_STATE_T *EGL_GET_SERVER_STATE(void)
{
   return &egl_server_state;
}

extern void egl_server_unlock(void);

#include "interface/khronos/egl/egl_int_impl.h"

extern void egl_khr_sync_update(void);
extern void egl_khr_fence_update(void);

#endif
