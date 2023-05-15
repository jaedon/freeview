/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Global image map

FILE DESCRIPTION
uint32_t to global image map.
=============================================================================*/

#include "interface/khronos/common/khrn_int_common.h"
#include "interface/khronos/common/khrn_client_platform.h"
#include "interface/khronos/include/EGL/egl.h"
#include "interface/khronos/include/EGL/eglext.h"

#if EGL_BRCM_global_image && EGL_KHR_image

static INLINE void acquire_value(uint64_t value)
{
   platform_acquire_global_image((uint32_t)value, (uint32_t)(value >> 32));
}

static INLINE void release_value(uint64_t value)
{
   platform_release_global_image((uint32_t)value, (uint32_t)(value >> 32));
}

#define KHRN_GENERIC_MAP_VALUE_NONE ((uint64_t)0)
#define KHRN_GENERIC_MAP_VALUE_DELETED ((uint64_t)-1)
#define KHRN_GENERIC_MAP_ACQUIRE_VALUE acquire_value
#define KHRN_GENERIC_MAP_RELEASE_VALUE release_value
#define KHRN_GENERIC_MAP_ALLOC khrn_platform_malloc
#define KHRN_GENERIC_MAP_FREE khrn_platform_free

#define CLIENT_GLOBAL_IMAGE_MAP_C
#include "interface/khronos/common/khrn_client_global_image_map.h"

#endif
