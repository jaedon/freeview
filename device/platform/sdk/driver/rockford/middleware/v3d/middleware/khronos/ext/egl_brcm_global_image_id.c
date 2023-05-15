/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  EGL Broadcom global image extension

FILE DESCRIPTION
We're not ever supposed to reuse ids, so this should not be built into the
khronos VLL (if it were, egl_brcm_global_image_id would reset every time we
reloaded the VLL). This should instead be linked statically with the main
image.
=============================================================================*/

#include "interface/khronos/common/khrn_int_common.h"

#include "interface/khronos/include/EGL/egl.h"
#include "interface/khronos/include/EGL/eglext.h"

#if EGL_BRCM_global_image

static uint64_t egl_brcm_global_image_id = 0;

void egl_brcm_global_image_id_get(int32_t *id)
{
   ++egl_brcm_global_image_id;
   id[0] = (uint32_t)egl_brcm_global_image_id;
   id[1] = (uint32_t)(egl_brcm_global_image_id >> 32);
}

#endif
