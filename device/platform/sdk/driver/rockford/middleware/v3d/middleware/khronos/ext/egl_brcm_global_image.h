/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  EGL Broadcom global image extension

FILE DESCRIPTION
Server-side implementation of EGL_BRCM_global_image.
=============================================================================*/

#include "middleware/khronos/common/khrn_mem.h"
#include "interface/khronos/include/EGL/egl.h"
#include "interface/khronos/include/EGL/eglext.h"

extern bool egl_brcm_global_image_init(void);
extern void egl_brcm_global_image_term(void);
extern bool egl_brcm_global_image_is_empty(void);
extern MEM_HANDLE_T egl_brcm_global_image_lookup(uint32_t id_0, uint32_t id_1);
typedef void (*EGL_BRCM_GLOBAL_IMAGE_ITERATE_CALLBACK_T)(MEM_HANDLE_T handle, void *);
extern void egl_brcm_global_image_iterate(EGL_BRCM_GLOBAL_IMAGE_ITERATE_CALLBACK_T callback, void *);
