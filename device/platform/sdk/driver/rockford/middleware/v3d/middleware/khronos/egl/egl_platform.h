/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Header file

FILE DESCRIPTION
Platform abstraction layer API declaration.
=============================================================================*/

#ifndef EGL_PLATFORM_H
#define EGL_PLATFORM_H

#include "interface/khronos/common/khrn_int_common.h"
#include "middleware/khronos/common/khrn_image.h"

#include "interface/khronos/include/EGL/egl.h"
#include "interface/khronos/include/EGL/eglext.h"

#define EGL_PLATFORM_WIN_NONE 0xffffffff

/*
   Schedule the given image to be displayed on the given window as soon as
   possible. The dimensions of the image should agree with the window
   dimensions most of the time, but if the window has changed size recently
   they may not. In this case it's up to the platform what it should do, but
   the most sensible thing is probably to scale the image (TODO: is this really
   true?)

   When the image comes onto the display, the return callback (as passed to
   egl_server_platform_init) should be called with argument cb_arg. At this
   point, the image previously being displayed on the given window is assumed to
   be finished with
*/

extern void egl_server_platform_display(uint32_t win, KHRN_IMAGE_T *image, uint32_t cb_arg);

/*
   Remove the image being displayed on the given window (if there is one).
   Return when the image is off the display

   When this function returns, we assume:
   - the image that was on the display is finished with
   - any callbacks for previous updates to the given window have returned
     (todo: what about the case where we switch windows?)
*/

extern void egl_server_platform_display_nothing_sync(uint32_t win, BEGL_WindowState *windowState);

/*
   Used on platforms with server-side pixmaps. Retrieves all of the relevant
   information about a pixmap.
*/

MEM_HANDLE_T egl_server_platform_create_pixmap_info(uint32_t pixmap, EGLint *error);

typedef void (*EGL_SERVER_RETURN_CALLBACK_T)(uint32_t cb_arg);

void egl_server_platform_init(EGL_SERVER_RETURN_CALLBACK_T return_callback);

extern void egl_server_platform_set_position(uint32_t handle, uint32_t position, uint32_t width, uint32_t height);
extern uint32_t egl_server_platform_create_buffer(KHRN_IMAGE_FORMAT_T format, uint32_t width, uint32_t height,
                                                  KHRN_IMAGE_CREATE_FLAG_T flags, BEGL_WindowState *windowState, BEGL_BufferUsage usage);
extern uint32_t egl_server_platform_get_buffer(EGLNativeWindowType win, KHRN_IMAGE_FORMAT_T format, uint32_t width, uint32_t height,
                                               KHRN_IMAGE_CREATE_FLAG_T flags, BEGL_WindowState *windowState, BEGL_BufferUsage usage);
extern BEGL_WindowState *egl_server_platform_create_window_state(uint32_t window);
extern void egl_server_platform_destroy_buffer(uint32_t bufHandle, BEGL_WindowState *windowState);

#endif
